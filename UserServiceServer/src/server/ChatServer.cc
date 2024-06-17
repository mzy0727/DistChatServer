#include "ChatServer.h"
#include "json.hpp"
using json = nlohmann::json;

#include <string>
#include <iostream>
using namespace std;

#include "ChatService.h"
#include "spdlog/spdlog.h"

ChatServer:: ChatServer(EventLoop* loop, const InetAddress& addr, const std::string& name)
    : _server(loop,addr,name),
      _loop(loop)
{
    // 注册连接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // 设置线程数量
    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
       // spdlog::info("Connection UP : {:s} ", conn->peerAddress().toIpPort().c_str());
        LOG_INFO << "Connection UP : " << conn->peerAddress().toIpPort().c_str();
    }
    else
    {
        // 客户端断开连接
       // LOG_INFO << "Connection DOWN : " << conn->peerAddress().toIpPort().c_str();
       // 客户端异常关闭
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
{
    std::string msg = buf->retrieveAllAsString();
    cout<<"msg:"<<msg<<endl;
    fixbug::ProxyRequest request;
    
    
   
    // LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
    //          << "data received at " << time.toFormattedString();
    // conn->send(msg);

    // 数据的反序列化
    request.ParseFromString(msg);
    // 根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
    cout<<"request:"<<request.type()<<" "<<request.msg()<<endl;
    // 达到的目的：完全解耦网络模块代码和业务模块的代码
    // 通过js["msgid"]获取=》业务handler=》conn js time
    auto msgHandler = ChatService::instance()->getHandler(request.type());
    // 回调消息对应绑定好的事件处理器，来执行相应的业务处理
    string str = request.msg();
    msgHandler(conn,str,time);

}