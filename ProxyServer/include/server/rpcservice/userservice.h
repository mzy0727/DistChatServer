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

struct LoginRes
{
    bool success;
    string errmsg;
};

struct RegRes
{
    bool success;
    int userid;
};

class UserService : public fixbug::UserServiceRpc
{
public:
    UserService()
    {
        // 连接redis服务器
        _redis.connect();
    }
    // 处理登录业务  id  pwd   pwd
    LoginRes Login(int userid, string password)
    {
        LoginRes loginres;
        User user = _userModel.query(userid);
        if (user.getId() == userid && user.getPwd() == password)
        {
            if (user.getState() == "online")
            {
                // 该用户已经登录，不允许重复登录
                loginres.success = false;
                loginres.errmsg = "this account is using, input another!";
            }
            else
            {
                // id用户登录成功后，向redis订阅channel(id)
                _redis.subscribe(userid);

                // 登录成功，更新用户状态信息 state offline=>online
                user.setState("online");
                _userModel.updateState(user);
                loginres.success = true;
            }
        }
        else
        {
            loginres.success = false;
            loginres.errmsg = "id or password is invalid!";
        }
        return loginres;
    }

    void Login(::google::protobuf::RpcController *controller,
               const ::fixbug::LoginRequest *request,
               ::fixbug::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        int userid = request->userid();
        string password = request->password();

        auto loginres = Login(userid, password);

        response->mutable_result()->set_success(loginres.success);
        response->mutable_result()->set_errmsg(loginres.errmsg);
        response->set_userid(userid);
        // response->set_username();

        done->Run();
    }

    // 处理注册业务  name  password
    RegRes Register(string username, string password)
    {
        RegRes res;
        User user;
        user.setName(username);
        user.setPwd(password);
        res.success = _userModel.insert(user);
        if (res.success)
        {
            res.userid = user.getId();
        }
        return res;
    }

    void Register(::google::protobuf::RpcController *controller,
                  const ::fixbug::RegisterRequest *request,
                  ::fixbug::RegisterResponse *response,
                  ::google::protobuf::Closure *done)
    {
        string username = request->username();
        string password = request->password();

        auto res = Register(username, password);

        if (res.success == false)
        {
            response->mutable_result()->set_errmsg("注册失败");
        }
        else
        {
            response->mutable_result()->set_errmsg("");
            response->set_userid(res.userid);
        }
        response->mutable_result()->set_success(res.success);

        done->Run();
    }

private:
    // redis操作对象
    Redis _redis;
    UserModel _userModel;
   // OfflineMsgModel _offlineMsgModel;
};