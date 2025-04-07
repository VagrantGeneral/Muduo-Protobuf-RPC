#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
#include "friend.pb.h"
#include <iostream>

int main(int argc, char** argv) {
    //初始化框架
    MprpcApplication::Init(argc, argv);

    //定义stub代理
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());

    /*例：调用rpc方法Login*/
    //控制
    MprpcController controller;

    //请求
    fixbug::GetFriendsListRequest request;
    request.set_userid(8418);
    
    //响应
    fixbug::GetFriendsListResponse response;

    //调用
    stub.GetFriendsList(&controller, &request, &response, nullptr);

    if (controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        //完成一次调用，读取调用结果
        if(response.result().errcode() == 0) {
            std::cout << "rpc GetFriendsList response success : " << std::endl;
            std::cout << "\tfriendsList { " << std::endl;
            int size = response.friends_size();
            for (int i = 0; i < size; i++) {
                std::cout << "\t\t" << "index[" << i << "] : " << response.friends(i) << std::endl;
            }
            std::cout << "\t}" << std::endl;
        }
        else {
            std::cout << "rpc GetFriendsList response ERROR : " << response.result().errmsg() << std::endl;
        }
    }

    

    return 0;
}    