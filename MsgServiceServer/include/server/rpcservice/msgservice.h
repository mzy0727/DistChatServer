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

class MsgService : public fixbug::MsgServiceRpc
{
public:
    MsgService()
    {
        // 连接redis服务器
        _redis.connect();
    }
    bool OneChat(int friendid, string msg)
    {
        // 查询toid是否在线
        User user = _userModel.query(friendid);
        if (user.getState() == "online")
        {
            _redis.publish(friendid, msg);
            return true;
        }

        // toid不在线，存储离线消息
       _offlineMsgModel.insert(friendid, msg);

        return true;
    }

    void OneChat(::google::protobuf::RpcController *controller,
                 const ::fixbug::OneChatRequest *request,
                 ::fixbug::OneChatResponse *response,
                 ::google::protobuf::Closure *done)
    {
        int friendid = request->friendid();
        string msg = request->chatmsg().SerializeAsString();
        bool ret = OneChat(friendid, msg);

        response->mutable_result()->set_success(ret);
        response->mutable_result()->set_errmsg("");

        done->Run();
    }

    vector<string> GetOfflineMsg(int userid)
    {
        vector<string> vec = _offlineMsgModel.query(userid);
        if (!vec.empty())
        {
            // 读取该用户的离线消息后，把该用户的所有离线消息删除掉
           _offlineMsgModel.remove(userid);
        }
        return vec;
    }

    void GetOfflineMsg(::google::protobuf::RpcController *controller,
                       const ::fixbug::GetOfflineMsgRequest *request,
                       ::fixbug::GetOfflineMsgResponse *response,
                       ::google::protobuf::Closure *done)
    {
        int userid = request->userid();
        vector<string> ret = GetOfflineMsg(userid);

        response->mutable_result()->set_success(true);
        response->mutable_result()->set_errmsg("");
        for (auto msg : ret)
        {
            fixbug::ChatMessage chatmsg;
            chatmsg.ParseFromString(msg);
            *(response->add_offlinemsglist()) = chatmsg;
        }

        done->Run();
    }

private:
    // redis操作对象
    Redis _redis;
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
};