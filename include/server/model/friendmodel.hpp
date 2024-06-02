#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"
#include <vector>
using namespace std;

// 提供操作Friend表的操作接口
class FriendModel
{
public:
    // 添加好友关系
    // 为了缓解服务器压力，一般把好友列表存在客户端
    void insert(int userid, int friendid); 

    // 返回用户的好友列表
    // 这个需要 user 表 和 friend 表的联合查询
    vector<User> query(int userid);

private:
    //

};

#endif