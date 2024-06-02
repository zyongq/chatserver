#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h> // 处理强制退出服务器等信号

using namespace std;

// 处理服务器 ctrl + C
void resetHandler(int) 
{
    //
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char** argv)
{
    signal(SIGINT, resetHandler);// c处理服务器的ctrl + C

    EventLoop loop;
    InetAddress addr("127.0.0.1", atoi(argv[2]));
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}