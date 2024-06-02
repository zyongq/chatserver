/*
muduo 网络库给用户提供了两个主要的类：TcpServer:用于编写服务器程序 TcpClient:用于编写服务器程序
muduo:就是把epoll + 线程池实现的
用第三方库的好处：将网络I/O的代码 和 业务代码区分开来
最大的好处：用户的连接和断开，用户的可读写实现才是你业务层需要关注的，至于如何监听由网络库实现

这个代码的框架是相当固定的，能改动的地方只有业务实现的具体细节
*/

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*
基于muduo开发网络server的程序的步骤：
1、组合TcpServer类
2、创建EventLoop时间循环对象的指针
3、明确TcpServer需要什么参数来构造（扒拉一下源码就行了）
4、给我的服务器注册用户连接的创建和断开的回调，用户读写时间的回调（这就是调包的魅力）
5、muduo本身就是epoll+多线程，可以设置服务器端的线程的数量，然后muduo会根据总线程数量自适应
*/

class ChatServer {
    private:
        TcpServer _server; // 掉包侠所作的一切事情就是修改TcpServer的某些回调函数和某些参数
        EventLoop* _loop; // 没有这个东西，你关都关不掉服务器程序

        void onConnection(const TcpConnectionPtr& conn) // 用于处理用户的连接和断开
        {
            if (conn->connected()) {
                cout << conn->peerAddress().toIpPort() << "->" << 
                    conn->localAddress().toIpPort() << " state:online" << endl;
            }
            else {
                cout << "state:offline" << endl;
                // 断开连接则释放fd
                conn->shutdown();
                // 服务端直接结束掉：
                // _loop->quit();
            }
        }
        // 专门处理用户的读写事件的函数
        void onMessage(const TcpConnectionPtr& conn, // 连接
                            Buffer* buffer, // 缓冲区,提高数据收发的性能
                            Timestamp time)  // 接收的数据的时间信息
        {
            // 回声处理
            string buf = buffer->retrieveAllAsString();
            cout << "rece: " << buf << " time : " << time.toString() << endl;
            // 回声
            conn->send(buf);
        }
    public:
        ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg) 
            : _server(loop, listenAddr, nameArg), _loop(loop)
        {
            // 注册用户连接和断开的回调处理函数，回调函数就是我的业务实现
            // 你给它写成了成员方法，就多了一个this指针，就要用绑定器适配一下，functional中
            _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
            _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
            _server.setThreadNum(4);// 一个处理新用户的链接，3个工作线程

        }
        void start() {
            _server.start();
        }

};

// 写个主函数测试一下
int main() {
    EventLoop loop; ///????????????????????????????????创建这个就很像创建了一个epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");
    // 开启服务器
    server.start(); // 做了些什么：把listen_fd 通过epoll_ctl添加到epoll上面

    // 以阻塞的方式 ： epoll_wait :等待用户连接、用户的读写事件、以及其他未被重写的事件
    loop.loop();

    return 0;
}