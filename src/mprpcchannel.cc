#include "mprpcchannel.h"

//所有通过stub代理的rpc方法都要经过这里，集中统一进行序列化以及网络发送
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                              google::protobuf::RpcController* controller,
                              const google::protobuf::Message* request,
                              google::protobuf::Message* response,
                              google::protobuf::Closure* done) {
    //获取服务名和方法名
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name();          // service_name +
    std::string method_name = method->name();       // method_name  +

    //获取参数序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str)) {
        // 序列化成功  
        args_size = args_str.size();                // args_size    +
    }
    else {
        controller->SetFailed("serialize request error!");
        return;
    }

    //定义rpc请求的header
    mprpc::RpcHeader rpcHeader;                     // rpcHeader
    rpcHeader.set_service_name(service_name); // -
    rpcHeader.set_method_name(method_name);   // -
    rpcHeader.set_args_size(args_size);       // -

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcHeader.SerializeToString(&rpc_header_str)) {
        header_size = rpc_header_str.size();        // header_size  rpc_header_str
    }
    else {
        // 序列化失败
        controller->SetFailed("serialize rpcHeader error!");
        return;
    }


    /**合成rpc请求的字符串
     * send_rpc_str {
     *      header_size
     *      rpc_header_str {
     *          service_name
     *          method_name
     *          args_size
     *      }
     *      args_str
     * }
     * 注：header_size 4字节
     */
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_size, 4));    // header_size
    send_rpc_str += rpc_header_str;                                 // rpc_header_str
    send_rpc_str += args_str;                                       // args_str

    /**使用tcp，完成rpc方法的远程调用*/
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        char errtxt[512] = {0};
        sprintf(errtxt, "socket error! errno : %d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    //从配置文件获取ip和端口
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    //地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    //连接服务节点
    if (connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        //连接失败
        close(clientfd);
        controller->SetFailed("connect error!");
    }

    //发送rpc请求
    if (send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1) {
        //发送失败
        close(clientfd);
        controller->SetFailed("send error!");
        return;
    }

    //接收rpc响应
    char recv_buff[1024] = {0};
    int  recv_size = 0;
    if ((recv_size = recv(clientfd, recv_buff, 1024, 0)) == -1) {
        //接收失败
        close(clientfd);
        controller->SetFailed("recv error!");
        return;
    }

    //反序列化rpc响应数据
    std::string response_str(recv_buff, 0, recv_size);
    if (!response->ParseFromString(response_str)) {
        //反序列化失败
        close(clientfd);
        char errtxt[1024] = {0};
        sprintf(errtxt, "parse recv_buff error! errno : %s", recv_buff);
        controller->SetFailed(errtxt);
        return;
    }

    close(clientfd);
}