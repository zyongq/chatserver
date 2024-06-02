#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <string>
#include <vector>
#include <map>
#include<iostream>

using namespace std;
using namespace muduo;

// #include <functional>

// 单例模式的实现
ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 构造函数， 注册消息以及对应的回调函数
ChatService::ChatService()
{
    // 绑定 信号 和 回调函数
    // 用户基本业务管理相关事件处理回调注册
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    // _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});

    // 群组业务管理相关事件处理回调注册
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});

    // 连接redis服务器
    if (_redis.connect()) 
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }

}


void ChatService::reset()
{
    // 把online -> offline
    _userModel.resetState();
}



// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid) 
{
    // 记录错误日志：没有对应的处理函数
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end()) 
    {
        // error log
        // by muduo lib
        // LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        // 要么抛出异常给进程干掉， 要么返回一个默认的处理器，什么都不做
        return [=](auto a, auto b, auto c) {
            LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        };

    }
    else
    {
        return _msgHandlerMap[msgid];
    }
    
}

// 登录业务-实现 name password
void ChatService::login(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    // 
    // LOG_INFO << "do login service!!!";
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id);
    if (user.getId() == id && user.getPassword() == pwd) 
    {
        if (user.getState() == "online") 
        {
            // 已经在线
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            // response["id"] = user.getId();
            response["errno"] = 1; // 失败
            // response["name"] = user.getName();
            response["errmsg"] = "该账号已登录， 请重新输入新账号";

            // return 
            conn->send(response.dump());
        }
        else
        {
            //
            // login success - 登录成功
            {
                // 锁的粒度越小越好，所以……
                // 记录用户连接信息
                // 线程安全 - 互斥锁
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // id用户登录成功后，向redis订阅channel（id）
            _redis.subscribe(id);

            // 更新用户信息
            user.setState("online");
            _userModel.updateState(user);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["id"] = user.getId();
            response["errno"] = 0; // 成功登录
            response["name"] = user.getName();

            // 查询是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty())
            {
                // 有离线消息
                response["offlinemsg"] = vec;

                // 读取之后把该用户地离线消息删除掉
                _offlineMsgModel.remove(id);
            }

            // 查询用户的好友信息并返回
            vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty())
            {
                // 有 好友
                vector<string> vec2;
                for (User& user : userVec) 
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }
            // return 
            conn->send(response.dump());
        }
    }
    else 
    {
        // login failed-用户不存在|| 用户存在但是密码错误
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        // response["id"] = user.getId();
        response["errno"] = 1; // 响应成功了，业务成功了
        // response["name"] = user.getName();
        response["errmsg"] = "用户名或者秘密错误";

        // return 
        conn->send(response.dump());
    }
}

// 注册业务-实现
void ChatService::reg(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    //
    // LOG_INFO << "do register service!!!";
    // name password -> register
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPassword(pwd);

    bool state = _userModel.insert(user);
    if (state)
    {
        // 注册成功 响应
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["id"] = user.getId();
        response["errno"] = 0; // 响应成功了，业务成功了

        // return to client
        conn->send(response.dump());
    }
    else 
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["id"] = user.getId();
        response["errno"] = 1; // 响应成功了，业务成功了

        // return to client
        conn->send(response.dump());
    }
}

// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid); 

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}


void ChatService::clientCloseException(const TcpConnectionPtr& conn)
{
    User user;

    // 用 conn 查找 id， ：修改userConnMap，数据库改为offline
    {
        // 同样需要线程安全
        lock_guard<mutex> lock(_connMutex);
        
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++)
        {
            if (it->second == conn)
            {
                // 找到了 
                // 定位 user
                user.setId(it->first);
                // 从map表把用户的连接信息删了
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 更新用户的状态信息 - 加一层判断减轻数据库的负担
    if (user.getId() != -1) 
    {
        user.setState("offline");
        _userModel.updateState(user);
    }

}

// 一对一聊天业务 --集群之前
// void ChatService::oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time)
// {
//     // 先 取出 对方的 id
//     // cout << js.dump() <<endl;
//     int toid = js["to"].get<int>();
//     {
//         // 保证 _userConnMap的线程安全
//         lock_guard<mutex> lock(_connMutex);
//         auto it = _userConnMap.find(toid);
//         if (it != _userConnMap.end()) {
//             // 对方在线，转发消息
//             // 原封不动 直接转发
//             it->second->send(js.dump());
//             return;
//         }
//     }
//     // toid 不在线，存储离线消息
//     _offlineMsgModel.insert(toid, js.dump());
    
// }

// 一对一聊天业务 --集群之后
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // cout << js.dump() << "  1" << endl;
    int toid = js["to"].get<int>();
    // cout << js.dump() <<endl;
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // toid在线，转发消息   服务器主动推送消息给toid用户
            it->second->send(js.dump());
            return;
        }
    }

    // 查询toid是否在线 
    User user = _userModel.query(toid);
    if (user.getState() == "online")
    {
        _redis.publish(toid, js.dump());
        return;
    }

    // toid不在线，存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

// 添加好友业务： msgid id friendid
void ChatService::addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    // 从json中取出数据
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    // 如果friendid不存在的话，又是另外的处理逻辑了

    // 存储好友信息
    _friendModel.insert(userid, friendid);

}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group))
    {
        // 存储群组创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}

// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(_connMutex);
    for (int id : useridVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 转发群消息
            it->second->send(js.dump());
        }
        else
        {
            // 查询toid是否在线 
            User user = _userModel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            else
            {
                // 存储离线群消息
                _offlineMsgModel.insert(id, js.dump()); 
            }
        }
    }
} 


// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return; 
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}

