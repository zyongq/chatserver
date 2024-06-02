#include "friendmodel.hpp"
#include "db.h"

// 添加好友关系
// 为了缓解服务器压力，一般把好友列表存在客户端
void FriendModel::insert(int userid, int friendid)
{
    //
    // 先组装 SQL语句，然后调用函数执行SQL语句，成功后获得生成的主键 id
    // 组装sql语句
    char sql[1024] = {0};
    // sprintf 用在这里很方便
    sprintf(sql, "insert into Friend values(%d, %d)", userid, friendid);
    
    // 定义一个MySQL对象来发射MySQL到数据库内核
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);

    }
}

// 返回用户的好友列表
// 这个需要 user 表 和 friend 表的联合查询
vector<User> FriendModel::query(int userid)
{
    //
    // 先组装 SQL语句，然后调用函数执行SQL语句，成功后获得生成的主键 id
    // 组装sql语句
    char sql[1024] = {0};
    // sprintf 用在这里很方便
    sprintf(sql, "select a.id,a.name,a.state from User a inner join Friend b on b.friendid=a.id where b.userid=%d", userid);
    
    vector<User> vec;// return this

    // 定义一个MySQL对象来发射MySQL到数据库内核
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) 
        {
            // 设置返回值
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) 
            {
                // 一个用户一个用户地填充
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            } 

            // 释放mysql资源
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}