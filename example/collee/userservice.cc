#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include <iostream>
#include <string>

/**UserService是一个本地服务，提供进程内的本地方法，Login和Register
 * 当它继承相对应的RPC类之后，
 */
class UserService : public fixbug::UserServiceRpc {
public:
    //本地方法：登录
    bool Login(std::string name, std::string pwd) {
        std::cout << "call local service : Login" << std::endl;
        std::cout << "name : " << name << std::endl;
        std::cout << "pwd  : " << pwd  << std::endl;
        return true;
    }

    //本地方法：注册
    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "call local service : Register" << std::endl;
        std::cout << "id   : " << id << std::endl;
        std::cout << "name : " << name << std::endl;
        std::cout << "pwd  : " << pwd  << std::endl;
        return true;
    }

    //重写UserServiceRpc中的虚函数Login，这将会是框架直接调用的方法 
    void Login(::google::protobuf::RpcController* controller,
                        const ::fixbug::LoginRequest* request,
                            ::fixbug::LoginResponse* response,
                            ::google::protobuf::Closure* done) {
        /*框架给业务上报了请求参数LoginRequest，业务获取相应的数据完成本地业务。*/
        //根据请求获取数据
        std::string name = request->name();
        std::string pwd = request->pwd();
        
        //完成本地业务
        bool login_result = Login(name, pwd); //本地业务

        //响应的写入
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("no error");
        response->set_sucess(login_result);
        
        //调用回调
        done->Run(); //执行响应消息的数据序列化以及网络发送。
    }

    //重写UserServiceRpc中的虚函数Register，这将会是框架直接调用的方法 
    void Register(::google::protobuf::RpcController* controller,
                        const ::fixbug::RegisterRequest* request,
                            ::fixbug::RegisterResponse* response,
                            ::google::protobuf::Closure* done) {
        /**/
        //根据请求获取数据
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        //完成本地业务
        bool Register_result = Register(id , name, pwd); //本地业务

        //响应的写入
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("no error");
        response->set_success(Register_result);

        //调用回调
        done->Run();
    }

};



//找到一个框架，使得类中的Login方法可以被同一个机器的其他进程调用。这就是RPC框架。



int main(int argc, char** argv) {
    //调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    //调用发布
    RpcProvider provider;
    provider.NotifyService(new UserService());

    //启动
    provider.Run();

    return 0;
}