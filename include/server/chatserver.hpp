#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

class ChatServer {
public:
    // 初始化聊天服务器对象-构造函数
    ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);

    // start server
    void start();

private:
    TcpServer _server; // has a TcpServer
    EventLoop* _loop; // 基本目的只有一个，在合适的时候退出程序

    // 回调 连接相关处理函数-声明
    void onConnection(const TcpConnectionPtr&);

    // 回调 消息相关处理函数-声明
    void onMessage(const TcpConnectionPtr&,
                Buffer*,
                Timestamp);
};


#endif