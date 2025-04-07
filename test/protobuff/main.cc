#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main() {
    //
}


int main2() {
    //封装数据1
    LoginResponse rsp;
    ResultCode* rc = rsp.mutable_result();
    rc->set_errcode(1);
    rc->set_errmsg("登录失败！！！");

    std::string recv_str;
    if (rsp.SerializeToString(&recv_str)) {
        std::cout << recv_str.c_str() << std::endl;
    }


    //封装数据2
    GetFriendListResponse rsps;
    ResultCode* rcs = rsps.mutable_result();//要修改，必须返回指针来操作。
    rcs->set_errcode(1);
    rcs->set_errmsg("登录失败！！！");
    
    //添加列表成员
    User* user1 = rsps.add_friend_list();
    user1->set_name("zhangsan");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    User* user2 = rsps.add_friend_list();
    user2->set_name("lisi");
    user2->set_age(29);
    user2->set_sex(User::MAN);

    //获取和打印
    std::cout << rsps.friend_list_size() << std::endl;
    User duser = rsps.friend_list(1);
    std::cout << duser.name() << std::endl;
    std::cout << duser.age() << std::endl;
    std::cout << duser.sex() << std::endl;
    std::string str;
    if (rsps.SerializeToString(&str)) {
        std::cout << str.c_str() << std::endl;
    }
    // std::string recv_str2;
    // if (rsps.SerializeToString(&recv_str2)) {
    //     std::cout << "1" << std::endl;
    //     std::cout << recv_str2.c_str() << std::endl;
    // }
}


int main1() {
    //封装数据
    LoginRequest req;
    req.set_name("HanKangKai");
    req.set_pwd("Itcast66^");

    //序列化
    std::string send_str;
    if (req.SerializeToString(&send_str)) {
        std::cout << send_str.c_str() << std::endl;
    }

    //反序列化
    LoginRequest reqB;
    if (reqB.ParseFromString(send_str)) {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }

    return 0;
}