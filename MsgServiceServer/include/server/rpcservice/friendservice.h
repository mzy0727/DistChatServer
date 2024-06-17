#pragma once

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "redis.h"
#include "GroupModel.h"
#include "FriendModel.h"
#include "UserModel.h"
#include "OfflineMsgModel.h"
#include "DistributedService.pb.h"
using namespace muduo;
using namespace muduo::net;
using namespace std;

class FriendService : public fixbug::FriendServiceRpc
{
public:
    std::vector<User> GetFriendList(int userid)
    {
        
       
        vector<User> userVec =  _friendModel.query(userid);
        return userVec;
    }

    void GetFriendList(::google::protobuf::RpcController *controller,
                       const ::fixbug::GetFriendListRequest *request,
                       ::fixbug::GetFriendListResponse *response,
                       ::google::protobuf::Closure *done)
    {
        int userid = request->userid();

        std::vector<User> friendList = GetFriendList(userid);

        response->mutable_result()->set_success(true);
        response->mutable_result()->set_errmsg("");
        for (auto friend_ : friendList)
        {
            auto *p = response->mutable_friendlist()->add_friend_();
            fixbug::User usertemp;
            usertemp.set_id(friend_.getId());
            usertemp.set_name(friend_.getName());
            usertemp.set_state(friend_.getState());
            *p = usertemp;
        }

        done->Run();
    }

    bool AddFriend(int userid, int friendid)
    {
        // 存储好友信息
        _friendModel.insert(userid, friendid);
        return true;
    }

    void AddFriend(::google::protobuf::RpcController *controller,
                   const ::fixbug::AddFriendRequest *request,
                   ::fixbug::AddFriendResponse *response,
                   ::google::protobuf::Closure *done)
    {
        int userid = request->userid();
        int friendid = request->friendid();

        bool ret = AddFriend(userid, friendid);

        if (ret == false)
        {
            response->mutable_result()->set_errmsg("添加好友失败");
        }
        else
        {
            response->mutable_result()->set_errmsg("");
        }
        response->mutable_result()->set_success(ret);

        done->Run();
    }
private:
    FriendModel _friendModel;
};