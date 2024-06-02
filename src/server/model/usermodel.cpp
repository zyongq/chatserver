#include "usermodel.hpp"
#include "db.h"
#include <iostream>

using namespace std;

bool UserModel::insert(User& user)
{
    // 先组装 SQL语句，然后调用函数执行SQL语句，成功后获得生成的主键 id
    // 组装sql语句
    char sql[1024] = {0};
    // sprintf 用在这里很方便
    sprintf(sql, "insert into User(name, password, state) values('%s', '%s', '%s')", 
        user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    
    // 定义一个MySQL对象来发射MySQL到数据库内核
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql)) 
        {
            // 获取一下插入成功的数据用户数据的主键id
            // 直接给 id 插入 到 user对象的数据
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }

    return false;
}

User UserModel::query(int id)
{
    // 先组装 SQL语句，然后调用函数执行SQL语句，成功后获得生成的主键 id
    // 组装sql语句
    char sql[1024] = {0};
    // sprintf 用在这里很方便
    sprintf(sql, "select * from User where id = %d",id);
        // user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    
    // 定义一个MySQL对象来发射MySQL到数据库内核
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);

                // 释放资源
                mysql_free_result(res);

                // return res;
                return user;
            }
        }
    }
    return User();
}

// update - defination
bool UserModel::updateState(User user)
{
    // 先组装 SQL语句，然后调用函数执行SQL语句，成功后获得生成的主键 id
    // 组装sql语句
    char sql[1024] = {0};
    // sprintf 用在这里很方便
    // update user set his state
    sprintf(sql, "update User set state = '%s' where id = %d", user.getState().c_str(), user.getId());
        // user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    
    // 定义一个MySQL对象来发射MySQL到数据库内核
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

void UserModel::resetState()
{
    //
    // 先组装 SQL语句，然后调用函数执行SQL语句，成功后获得生成的主键 id
    // 组装sql语句
    char sql[1024] = {0};
    // sprintf 用在这里很方便
    // update user set his state
    sprintf(sql, "update User set state = 'offline' where state='online'");
        // user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());
    
    // 定义一个MySQL对象来发射MySQL到数据库内核
    MySQL mysql;
    if (mysql.connect())
    {
       mysql.update(sql);
 
    }
}

