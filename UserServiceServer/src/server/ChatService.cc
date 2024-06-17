#include "ChatService.h"
#include "public.h"
#include <muduo/base/Logging.h>
#include <vector>
using namespace std;
using namespace muduo;

// 获取单例对象的接口函数
ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 注册消息以及对应的Handler回调操作
ChatService::ChatService()
{
    // 用户基本业务管理相关事件处理回调注册
    _msgHandlerMap.insert({LOGIN, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGINOUT, std::bind(&ChatService::loginout, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    // _msgHandlerMap.insert({ONE_CHAT, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    // _msgHandlerMap.insert({ADD_FRIEND, std::bind(&ChatService::addFriend, this, _1, _2, _3)});

    // // 群组业务管理相关事件处理回调注册
    // _msgHandlerMap.insert({CREATE_GROUP, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    // _msgHandlerMap.insert({ADD_GROUP, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    // _msgHandlerMap.insert({GROUP_CHAT, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
    // _msgHandlerMap.insert({GET_FRIENDLIST, std::bind(&ChatService::GetFriendList, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    // _msgHandlerMap.insert({GET_GROUPLIST, std::bind(&ChatService::GetGroupList, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    // _msgHandlerMap.insert({GET_OFFLINEMSG, std::bind(&ChatService::GetOfflineMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});

    // 连接redis服务器
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

// 服务器异常，业务重置方法
void ChatService::reset()
{
    // 把online状态的用户，设置成offline
    _userModel.resetState();
}

// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    // 记录错误日志，msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 返回一个默认的处理器，空操作
        return [=](const TcpConnectionPtr &conn, string &requeststr, Timestamp) {
            LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

// 处理登录业务  id  pwd   pwd
void ChatService::login(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    fixbug::LoginRequest loginreuest;
    loginreuest.ParseFromString(requeststr);
    // rpc方法的响应
    fixbug::LoginResponse loginresponse;
    // 发起rpc方法的调用 同步的rpc调用过程 MprpcChannel::callmethod
    MprpcController controller;
    //RpcChannel->Rpcchannel::callMethod 集中来做所有rpc方法调用端参数序列化和网络发送
    stub.Login(&controller,&loginreuest,&loginresponse,nullptr);

    // 一次rpc调用完成，读调用的结果
    if(controller.Failed()){
        LOG_ERROR<< controller.ErrorText();
    }
    if(loginresponse.result().success()){
        // 登录成功，记录用户连接信息
        {
            lock_guard<mutex> lock(_connMutex);
            _userConnMap.insert({loginreuest.userid(), conn});
        }
    }

    fixbug::ProxyRequest proxyrequest;
    proxyrequest.set_type(LOGIN_ACK);
    proxyrequest.set_msg(loginresponse.SerializeAsString());
    conn->send(proxyrequest.SerializeAsString());
   
    
}

// 处理注册业务  name  password
void ChatService::reg(const TcpConnectionPtr &conn, string &requeststr,  Timestamp time)
{
    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    fixbug::RegisterRequest registerrequest;
    registerrequest.ParseFromString(requeststr);
    fixbug::RegisterResponse registerresponse;

    MprpcController controller;
    stub.Register(&controller, &registerrequest, &registerresponse, nullptr);

    if (controller.Failed())
    {
         LOG_ERROR<< controller.ErrorText();
    }
    
    fixbug::ProxyRequest proxyrequest;
    proxyrequest.set_type(REG_ACK);
    proxyrequest.set_msg(registerresponse.SerializeAsString());
    conn->send(proxyrequest.SerializeAsString());

    
}

// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, string &requeststr,  Timestamp time)
{
   fixbug::LoginoutRequest loginoutrequest;
    loginoutrequest.ParseFromString(requeststr);
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(loginoutrequest.userid());
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(loginoutrequest.userid());

    // 更新用户的状态信息
    User user(loginoutrequest.userid(), "", "", "offline");
    _userModel.updateState(user);
}

// // 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 从map表删除用户的链接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(user.getId()); 

    // 更新用户的状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

// // 一对一聊天业务
// void ChatService::oneChat(const TcpConnectionPtr &conn,string &requeststr,  Timestamp time)
// {
//     fixbug::OneChatRequest onechatrequest;
//     onechatrequest.ParseFromString(requeststr);
//     fixbug::OneChatResponse onechatresponse;
//     {
//         lock_guard<mutex> lock(_connMutex);
//         auto it = _userConnMap.find(onechatrequest.friendid());
//         if (it != _userConnMap.end())
//         {
//             // 如果好友在线，将聊天消息封装到 protobuffer 中，并序列化为字符串后发送给好友
//             {  
//                 fixbug::protobuffer buffer;
//                 buffer.set_protobuftype(ONE_CHAT_MSG);
//                 fixbug::ChatMessage cmsg = onechatrequest.chatmsg();
//                 buffer.set_protobufstr(cmsg.SerializeAsString());
//                 string responsestr = buffer.SerializeAsString();
//                 it->second->send(responsestr);
//             }
//             // 发送确认响应给客户端，表示消息已成功转发
//             {
//                 onechatresponse.mutable_result()->set_success(true);
//                 fixbug::protobuffer buffer;
//                 buffer.set_protobuftype(ONE_CHAT_ACK);
//                 buffer.set_protobufstr(onechatresponse.SerializeAsString());
//                 string responsestr = buffer.SerializeAsString();
//                 conn->send(responsestr);
//             }

//             return;
//         }
//     }
//     // 如果好友不在线，使用 RPC 调用 OneChat 服务，将消息通过远程服务处理。处理完成后，将响应结果发送给客户端
//     fixbug::MsgServiceRpc_Stub stub(new MprpcChannel());

//     MprpcController controller;
//     stub.OneChat(&controller, &onechatrequest, &onechatresponse, nullptr);

//     if (controller.Failed())
//     {
//         LOG_INFO << controller.ErrorText();
//     }
//     fixbug::protobuffer buffer;
//     buffer.set_protobuftype(ONE_CHAT_ACK);
//     buffer.set_protobufstr(onechatresponse.SerializeAsString());
//     string responsestr = buffer.SerializeAsString();
//     conn->send(responsestr);
// }

// // 添加好友业务 msgid id friendid
// void ChatService::addFriend(const TcpConnectionPtr &conn, string &requeststr,  Timestamp time)
// {
//     fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());

//     fixbug::AddFriendRequest request;
//     request.ParseFromString(requeststr);
//     fixbug::AddFriendResponse response;

//     MprpcController controller;
//     stub.AddFriend(&controller, &request, &response, nullptr);

//     if (controller.Failed())
//     {
//         LOG_INFO << controller.ErrorText();
//     }

//     fixbug::protobuffer buffer;
//     buffer.set_protobuftype(ADD_FRIEND_ACK);
//     buffer.set_protobufstr(response.SerializeAsString());
//     string responsestr = buffer.SerializeAsString();
//     conn->send(responsestr);
// }

// // 创建群组业务
// void ChatService::createGroup(const TcpConnectionPtr &conn, string &requeststr,  Timestamp time)
// {
//     fixbug::GroupServiceRpc_Stub stub(new MprpcChannel());

//     fixbug::CreateGroupRequest request;
//     request.ParseFromString(requeststr);
//     fixbug::CreateGroupResponse response;

//     MprpcController controller;
//     stub.CreateGroup(&controller, &request, &response, nullptr);

//     if (controller.Failed())
//     {
//          LOG_INFO << controller.ErrorText();
//     }

//     fixbug::protobuffer buffer;
//     buffer.set_protobuftype(CREATE_GROUP_ACK);
//     buffer.set_protobufstr(response.SerializeAsString());
//     string responsestr = buffer.SerializeAsString();
//     conn->send(responsestr);
// }

// // 加入群组业务
// void ChatService::addGroup(const TcpConnectionPtr &conn, string &requeststr,  Timestamp time)
// {
//     fixbug::GroupServiceRpc_Stub stub(new MprpcChannel());

//     fixbug::AddGroupRequest request;
//     request.ParseFromString(requeststr);
//     fixbug::AddGroupResponse response;

//     MprpcController controller;
//     stub.AddGroup(&controller, &request, &response, nullptr);

//     if (controller.Failed())
//     {
//        LOG_INFO << controller.ErrorText();
//     }

//     fixbug::protobuffer buffer;
//     buffer.set_protobuftype(ADD_GROUP_ACK);
//     buffer.set_protobufstr(response.SerializeAsString());
//     string responsestr = buffer.SerializeAsString();
//     conn->send(responsestr);
// }

// // 群组聊天业务
// void ChatService::groupChat(const TcpConnectionPtr &conn, string &requeststr,  Timestamp time)
// {
//     fixbug::GroupChatRequest groupchatrequest;
//     groupchatrequest.ParseFromString(requeststr);
//     fixbug::GroupChatResponse groupchatresponse;

//     vector<int> useridVec = _groupModel.queryGroupUsers(groupchatrequest.userid(), groupchatrequest.groupid());
//     fixbug::MsgServiceRpc_Stub stub(new MprpcChannel());
//     lock_guard<mutex> lock(_connMutex);
//     for (auto id : useridVec)
//     {
//         auto it = _userConnMap.find(id);
//         if (it != _userConnMap.end())
//         {
//             // 转发群消息
//             fixbug::protobuffer buffer;
//             buffer.set_protobuftype(GROUP_CHAT_MSG);
//             buffer.set_protobufstr(groupchatrequest.chatmsg().SerializeAsString());
//             string responsestr = buffer.SerializeAsString();
//             it->second->send(responsestr);
//         }
//         else
//         {
//             fixbug::OneChatRequest onechatrequest;
//             onechatrequest.set_userid(groupchatrequest.userid());
//             onechatrequest.set_friendid(id);
//             *onechatrequest.mutable_chatmsg() = groupchatrequest.chatmsg();
//             fixbug::OneChatResponse onechatresponse;

//             MprpcController controller;
//             stub.OneChat(&controller, &onechatrequest, &onechatresponse, nullptr);

//             // 一次rpc调用完成，读调用的结果   网络传输出错 序列反序列化出错
//             if (controller.Failed())
//             {
//                 LOG_INFO << controller.ErrorText();
//             }
//         }
//     }

//     groupchatresponse.mutable_result()->set_success(true);
//     groupchatresponse.mutable_result()->set_errmsg("");
//     fixbug::protobuffer buffer;
//     buffer.set_protobuftype(GROUP_CHAT_ACK);
//     buffer.set_protobufstr(groupchatresponse.SerializeAsString());
//     string responsestr = buffer.SerializeAsString();
//     conn->send(responsestr);
// }

// // 获取当前用户的离线消息  个人聊天信息或者群组消息
// void ChatService::GetOfflineMsg(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
// {
//     fixbug::MsgServiceRpc_Stub stub(new MprpcChannel());

//     fixbug::GetOfflineMsgRequest request;
//     request.ParseFromString(requeststr);
//     fixbug::GetOfflineMsgResponse response;

//     MprpcController controller;
//     stub.GetOfflineMsg(&controller, &request, &response, nullptr);

//     if (controller.Failed())
//     {
//         LOG_INFO << controller.ErrorText();
//     }

//     fixbug::protobuffer buffer;
//     buffer.set_protobuftype(GET_OFFLINEMSG_ACK);
//     buffer.set_protobufstr(response.SerializeAsString());
//     string responsestr = buffer.SerializeAsString();
//     conn->send(responsestr);
// }

// // 获取用户的所有好友信息
// void ChatService::GetFriendList(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
// {
//     fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());

//     fixbug::GetFriendListRequest request;
//     request.ParseFromString(requeststr);
//     fixbug::GetFriendListResponse response;

//     MprpcController controller;
//     stub.GetFriendList(&controller, &request, &response, nullptr);

//     if (controller.Failed())
//     {
//         LOG_INFO << controller.ErrorText();
//     }

//     fixbug::protobuffer buffer;
//     buffer.set_protobuftype(GET_FRIENDLIST_ACK);
//     buffer.set_protobufstr(response.SerializeAsString());
//     string responsestr = buffer.SerializeAsString();
//     conn->send(responsestr);
// }
// // 获取用户加入的所有群组信息
// void ChatService::GetGroupList(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
// {
//     fixbug::GroupServiceRpc_Stub stub(new MprpcChannel());

//     // 演示调用远程发布的rpc方法Login
//     fixbug::GetGroupListRequest request;
//     request.ParseFromString(requeststr);

//     fixbug::GetGroupListResponse response;

//     MprpcController controller;
//     // 发起rpc方法的调用 RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
//     stub.GetGroupList(&controller, &request, &response, nullptr);

//     // 一次rpc调用完成，读调用的结果   网络传输出错 序列反序列化出错
//     if (controller.Failed())
//     {
//         LOG_INFO << controller.ErrorText();
//     }

//     fixbug::protobuffer buffer;
//     buffer.set_protobuftype(GET_GROUPLIST_ACK);
//     buffer.set_protobufstr(response.SerializeAsString());
//     string responsestr = buffer.SerializeAsString();
//     conn->send(responsestr);
// }
// 从redis消息队列中获取订阅的消息
// 当监听的通道有消息时，意味着有转发而来的消息，再转发给客户端 
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    // fixbug::ProxyRequest request;
    // request.ParseFromString(msg);
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}