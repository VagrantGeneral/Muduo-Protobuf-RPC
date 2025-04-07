#pragma once
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>

//用于发布rpc服务的网络对象类
class RpcProvider {
public:
    //通知，这是框架提供给外部使用的，发布rpc方法的函数接口。
    void NotifyService(google::protobuf::Service* service); // 接受任意的业务，因此将它们的抽象类作为参数。

    //启动rpc服务节点，开始提供rpc远程调用服务
    void Run();
    

private:
    //连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    //读写回调
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp ts);

    //Closure的回调，用于序列化rpc的响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response);

private:
    //std::unique_ptr<muduo::net::TcpServer> m_tcpserverPtr;//用于网络的muduo-tcpserver网络服务器。
    muduo::net::EventLoop m_eventLoop; // 用于事件循环的loop对象。

    //service服务类型信息
    struct ServiceInfo {
        google::protobuf::Service* m_service;                                               // 保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;   // 保存服务方法
    };
    std::unordered_map<std::string, ServiceInfo> m_serviceMap; // 存储注册成功的服务对象及其方法和信息
};