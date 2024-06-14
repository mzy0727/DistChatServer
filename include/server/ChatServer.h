#pragma once

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
using namespace muduo;
using namespace muduo::net;

// using namespace tiny_network;
// using namespace tiny_network::net;
// using namespace tiny_network::logger;

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