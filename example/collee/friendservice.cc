#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"
#include <iostream>
#include <string>
#include <vector>

/**FriendService是一个本地服务，提供进程内的本地方法，GetFriendLists
 * 当它继承相对应的RPC类之后，
 */
class FriendService : public fixbug::FriendServiceRpc {
public:
    //本地方法：获取好友列表
    std::vector<std::string> GetFriendsList(uint32_t userid) {
        std::cout << "call local service : GetFriendsList" << std::endl;
        std::cout << "userid : " << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("hankangkai");
        vec.push_back("wandergeneral");
        vec.push_back("cananandphysic");
        return vec;
    }

    //重写FriendServiceRpc中的虚函数GetFriendsList，这将会是框架直接调用的方法 
    void GetFriendsList(::google::protobuf::RpcController* controller,
              const ::fixbug::GetFriendsListRequest* request,
                  ::fixbug::GetFriendsListResponse* response,
                           ::google::protobuf::Closure* done) {
        //根据请求获取数据
        google::protobuf::uint32 userid = request->userid();
        
        //完成本地业务
        std::vector<std::string> friendsList = GetFriendsList(userid);

        //响应的写入
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("no error");
        for (std::string& name : friendsList) {
            std::string* p = response->add_friends();
            (*p) = name;
        }
        
        //调用回调
        done->Run(); //执行响应消息的数据序列化以及网络发送。
    }
};

int main(int argc, char** argv) {
    //调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    LOG_INFO("first log");

    LOG_ERROR("second %s In File %s :Func %s :Line %d", "log", __FILE__, __FUNCTION__, __LINE__);

    //调用发布
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    //启动节点，进入阻塞，等待远程的rpc调用请求
    provider.Run();

    return 0;
}