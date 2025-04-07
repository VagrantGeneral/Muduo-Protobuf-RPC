#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "user.pb.h"
#include <iostream>

int main(int argc, char** argv) {
    //初始化框架
    MprpcApplication::Init(argc, argv);

    //定义stub代理
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    /*例：调用rpc方法Login*/

    //请求
    fixbug::LoginRequest request;
    request.set_name("hankangkai");
    request.set_pwd("Itcast66^");
    //响应
    fixbug::LoginResponse response;

    //调用
    stub.Login(nullptr, &request, &response, nullptr);

    //完成一次调用，读取调用结果
    if(response.result().errcode() == 0) {
        std::cout << "rpc login response success : " << response.sucess() << std::endl;
    }
    else {
        std::cout << "rpc login response ERROR : " << response.result().errmsg() << std::endl;
    }

    /*例：调用rpc方法Register*/

    //请求
    fixbug::RegisterRequest request_s;
    request_s.set_id(2000);
    request_s.set_name("wandergeneral");
    request_s.set_pwd("hkk123456789");
    //响应
    fixbug::RegisterResponse response_s;

    //调用
    stub.Register(nullptr, &request_s, &response_s, nullptr);

    //完成一次调用，读取调用结果
    if(response_s.result().errcode() == 0) {
        std::cout << "rpc register response success : " << response_s.success() << std::endl;
    }
    else {
        std::cout << "rpc register response ERROR : " << response_s.result().errmsg() << std::endl;
    }

    return 0;
}    