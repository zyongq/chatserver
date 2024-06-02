#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

// User 表 的数据操作类 
// 与 业务无关 之 增删改查
class UserModel
{

public:

    // 注册业务的数据操作 - User表的增加方法
    bool insert(User& user);

    // 根据用户的id返回 User
    User query(int id);

    // 更新用户的状态信息
    bool updateState(User user);

    // reset 用户的状态
    void resetState();

private:
    //

};

#endif