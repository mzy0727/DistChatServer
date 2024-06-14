#include "ChatServer.h"
#include "ChatService.h"
#include <iostream>
#include <signal.h>
#include "spdlog/spdlog.h"
using namespace std;

// 处理服务器ctrl+c结束后，重置user的状态信息
void resetHandler(int){
    ChatService::instance()->reset();
    spdlog::info("Welcome to ctrl+c");
    exit(-1);
}
int main()
{
    signal(SIGINT,resetHandler);

    EventLoop loop;
    InetAddress listenAddr(8888);
    // Create a ChatServer object
    ChatServer server(&loop, listenAddr,"ChatServer");
    // Start the server
    server.start();

    // 开启事件循环
    loop.loop();
    return 0;
}
