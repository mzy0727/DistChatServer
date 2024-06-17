#pragma once

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
using namespace muduo;
using namespace muduo::net;

// using namespace tiny_network;
// using namespace tiny_network::net;
// using namespace tiny_network::logger;

#include <mprpc/mprpcapplication.h>
#include <mprpc/rpcprovider.h>
#include "friendservice.h"
#include "groupservice.h"
#include "msgservice.h"
#include "userservice.h"
#include "DistributedService.pb.h"

class ChatServer{
public:
    // 初始化聊天服务器
    ChatServer(EventLoop* loop, const InetAddress& addr, const std::string& name);
    // 启动服务
    void start();

       
private:
    // 上报连接相关信息的回调函数
    void onConnection(const TcpConnectionPtr&);
    // 上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr&, Buffer*, Timestamp);

    EventLoop *_loop;
    TcpServer _server;
};

class RpcServer{
public:
    void start(int argc, char **argv){
        // 调用框架的初始化操作 provider -i config.conf
        MprpcApplication::Init(argc,argv);
        // provider是一个rpc网络服务对象，把UserService对象发布到rpc节点上
        //RpcProvider provider;
        provider.NotifyService(new UserService());
        provider.NotifyService(new FriendService());
        provider.NotifyService(new GroupService());
        provider.NotifyService(new MsgService());
        provider.Run();
    }

private:
    RpcProvider provider;
};