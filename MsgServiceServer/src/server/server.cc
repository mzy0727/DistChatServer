#include "ChatServer.h"
#include "ChatService.h"
#include <iostream>
#include <signal.h>
#include "spdlog/spdlog.h"
#include "config.h"
#include <mprpc/mprpcapplication.h>
#include <mprpc/rpcprovider.h>

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
    RpcProvider provider;
    provider.NotifyService(new MsgService());
    provider.Run();
    return 0;
}
