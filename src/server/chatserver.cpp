#include "chatserver.hpp"

#include <functional> // 绑定器
#include "chatservice.hpp"

using namespace std;
using namespace placeholders;

#include <string> 
#include "json.hpp"
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg)
            : _server(loop, listenAddr, nameArg)            
{
    // 注册 相关事件的回调函数
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));

    // 注册 消息事件的回调函数
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置 线程数
    _server.setThreadNum(4);

}

void ChatServer::start() {
    _server.start();
}



// 回调 连接相关处理函数-声明
void ChatServer::onConnection(const TcpConnectionPtr& conn) 
{
    // 连接失败的处理，成功不用处理
    if(!conn->connected()) 
    {
        // 客户端没有一个合法的json 异常退出的处理：
        // 测试过后，这个并不能处理异常的json，而是ctrl + ] 的异常退出的处理
        // 异常json 可以放在客户端处理，减小服务端的复杂度
        ChatService::instance()->clientCloseException(conn);

        // 客户端断开连接的处理
        conn->shutdown(); // 释放socket资源
    }
}

// 回调 消息相关处理函数-声明
void ChatServer::onMessage(const TcpConnectionPtr& conn,
            Buffer* buffer,
            Timestamp time)
{
    // 接收消息
    string buf = buffer->retrieveAllAsString();

    // 用json来反序列化
    json js = json::parse(buf);
    
    // 通过js['megid'] 绑定回调操作，将业务层和网络层解耦
    // switch(megid) 的话 业务层 和网络层 就糅杂到一起了
    // 像下面这样在网络层就见不到业务层的函数名称
    // 网络层 到 业务层 的通信 就是使用这样的一个 int 值 来回调 业务层的函数
    // 像这样的话 不管 后面业务层的代码怎么改动，这个网络层的代码都是不用改动的
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn, js, time);

}




