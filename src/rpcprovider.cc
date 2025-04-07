#include "rpcprovider.h"

/**
 * service_name ==> service描述 ==> service* 记录服务对象 ==> (method_name >> method方法对象)
 */

//通知，这是框架提供给外部使用的，发布rpc方法的函数接口。
void RpcProvider::NotifyService(google::protobuf::Service* service) {
    ServiceInfo service_info;

    //获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();
    
    //获取服务名称
    std::string service_name = pserviceDesc->name();
    
    LOG_INFO("service_name : %s", service_name.c_str()); //std::cout << "service_name : " << service_name << std::endl;

    //获取服务对象的方法的数量
    int methodCnt = pserviceDesc->method_count();
    //循环获取服务对象的方法 并将其进行保存
    for (int i = 0; i < methodCnt; i++) {
        //获取服务对象指定下标的服务方法的描述（抽象描述）
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        //获取服务对象的方法的名称
        std::string method_name = pmethodDesc->name();

        LOG_INFO("method_name : %s", method_name.c_str()); //std::cout << "method_name : " << method_name << std::endl;

        //添加方法
        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }
    service_info.m_service = service;

    m_serviceMap.insert({service_name, service_info}); // 添加服务对象到服务对象表

}

//启动rpc服务节点，开始提供rpc远程调用服务
void RpcProvider::Run() {
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    //创建tcpserver
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    //绑定【连接回调】和【消息读写回调】方法------分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1
                                                                     , std::placeholders::_2
                                                                     , std::placeholders::_3));
    //设置muduo库线程池线程数量
    server.setThreadNum(4);
    //启动网络服务
    std::cout << "" << ip << " port : " << port << std::endl;
    LOG_INFO("RpcProvider start service at ip : %s port : %d", ip.c_str(), port);
    server.start();
    m_eventLoop.loop();

}

//连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {
        //连接已断开
        conn->shutdown();
    }
}

/**
 * RpcProvider和RpcConsumer协商好之间的通信用的protobuf数据类型。
 * service_name
 * method_name
 * args_size
 * args
 * 
 * header_size(4字节) + header_str + args_str
 * 二进制
 */

//读写回调
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp ts) {
    //接收到的rpc远程调用的字符流
    std::string recv_buff = buffer->retrieveAllAsString();

    //提取前4个字节
    uint32_t header_size = 0;
    recv_buff.copy((char*)&header_size, 4, 0);
    //根据header_size提取header_str数据
    std::string rpc_header_str = recv_buff.substr(4, header_size);

    mprpc::RpcHeader rpcHeader;
    std::string service_name;   // ++++++
    std::string method_name;    // ++++++
    uint32_t args_size;         // ++++++
    if (rpcHeader.ParseFromString(rpc_header_str)) {
        //反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else {
        //反序列化失败
        std::cout << "rpc_header_str : " << rpc_header_str << "Parse ERROR!" << std::endl;
        return;
    }

    //获取参数数据
    std::string args_str = recv_buff.substr(4 + header_size, args_size);

    //获取service---------------------------------------------------------------------------------
    auto iter = m_serviceMap.find(service_name);
    if (iter == m_serviceMap.end()) {
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    google::protobuf::Service* service = iter->second.m_service;        //获取服务对象

    //获取method
    auto miter = iter->second.m_methodMap.find(method_name);
    if (miter == iter->second.m_methodMap.end()) {
        std::cout << service_name << " : " << method_name << " is not exist!" << std::endl;
        return;
    }

    const google::protobuf::MethodDescriptor* method = miter->second;   // 获取方法

    //生成rpc方法调用的请求和响应参数
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) {
        std::cout << "request Parse ERROR!" << " args_str : " << args_str << std::endl;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();
    // if (!0){

    // }

    //为method方法的回调绑定一个Closure的回调函数
    google::protobuf::Closure* done = 
            google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                                         (this, &RpcProvider::SendRpcResponse, conn, response);

    //根据远端rpc请求，调用当前节点发布的方法
    service->CallMethod(method, nullptr, request, response, done);

}

//Closure的回调，用于序列化rpc的响应和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    std::string response_str;
    // 对response进行序列化
    if (response->SerializeToString(&response_str)) {
        // 序列化成功，将rpc执行的结果发送给调用方
        conn->send(response_str);
    }
    else {
        std::cout << "serialize response_str ERROR!" << std::endl;
    }

    conn->shutdown(); // 主动断开连接
}