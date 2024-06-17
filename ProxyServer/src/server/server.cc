#include "ChatServer.h"
#include "ChatService.h"
#include <iostream>
#include <signal.h>
#include "spdlog/spdlog.h"
#include "config.h"
#include <mprpc/mprpcapplication.h>

using namespace std;

// 处理服务器ctrl+c结束后，重置user的状态信息
void resetHandler(int){
    ChatService::instance()->reset();
    spdlog::info("Welcome to ctrl+c");
    exit(-1);
}
int main(int argc, char **argv)
{

    signal(SIGINT,resetHandler);

    MprpcApplication::Init(argc,argv);
     uint16_t port = atoi(MprpcApplication::GetInstance()->GetConfig().Load("chatserverport").c_str());
    string ip = MprpcApplication::GetInstance()->GetConfig().Load("rpcserverip");

    
    EventLoop loop;
    cout<<"ip:"<<ip<<" port: "<<port;
    InetAddress listenAddr(ip,port);
    // Create a ChatServer object
    ChatServer server(&loop, listenAddr,"ChatServer");
    // Start the server
    server.start();

    // 开启事件循环
    loop.loop();

    // RpcServer rpcserver_;
    // rpcserver_.start(argc,argv);
    return 0;
}
