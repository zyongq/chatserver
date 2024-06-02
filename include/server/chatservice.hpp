#ifndef CHATSERVICE_H
#define CHATSERCIVE_H

#include <unordered_map>
#include <functional>
#include <muduo/net/TcpClient.h>
#include "json.hpp"
#include "usermodel.hpp"
#include <mutex> // 互斥锁
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"


using json = nlohmann::json;
using namespace muduo::net;
using namespace muduo;
using namespace std;
// 消息类型 与 处理事件的绑定
using MsgHandler = std::function<void(const TcpConnectionPtr& conn, json& js, Timestamp time)>;

// 这里实现服务端的业务类：实现业务功能
// 一个对象就够了：使用单例模式
class ChatService
{
public:
    //给message id 映射 回调函数 = 事件处理函数
    // 登录业务
    void login(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 注册业务
    void reg(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 一对一聊天业务 - 由网络层根据 msgid来派发回调函数，所以参数一致
    void oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    
    // 添加好友业务
    void addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 添加群组业务
    void createGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);

    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 服务器异常，业务数据重置
    void reset();

    // 单例模式 中 单例 的创建
    static ChatService* instance();

    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);

    // 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);


    // 客户端异常退出的处理函数：
    void clientCloseException(const TcpConnectionPtr& conn);

    // // 获取消息对应的处理器
    // MsgHandler getHandler(int msgid);

    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);
    
private:
    // 客户端发来的 message 的类型 与对应 的处理函数
    unordered_map<int, MsgHandler> _msgHandlerMap;

    // 单例模式 - 把 构造函数放到private
    ChatService();

    // 存储在线用户的通信连接：因为服务器要向客户端发送信息
    // 这是个运行时动态变化的，记得线程安全的处理
    unordered_map<int, TcpConnectionPtr> _userConnMap; // 这个可能会被多个线程调用 ，要考虑线程安全问题
    // 互斥锁保护 _userConnMap
    mutex _connMutex;

    // 数据操作类对象
    UserModel _userModel;

    // 离线消息
    OfflineMsgModel _offlineMsgModel;

    // 好友表的操作对象
    FriendModel _friendModel;

    // 处理群组相关的操作对象
    GroupModel _groupModel;

    // redis 对象
    Redis _redis;

};

#endif
