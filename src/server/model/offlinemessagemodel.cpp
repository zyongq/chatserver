#include "offlinemessagemodel.hpp"
#include <string>
#include <vector>
#include <db.h>

using namespace std;

void OfflineMsgModel::insert(int userid, string msg)
{
    //
    // 先组装 SQL语句，然后调用函数执行SQL语句，成功后获得生成的主键 id
    // 组装sql语句
    char sql[1024] = {0};
    // sprintf 用在这里很方便
    sprintf(sql, "insert into OfflineMessage values(%d, '%s')", userid, msg.c_str());
    
    // 定义一个MySQL对象来发射MySQL到数据库内核
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);

    }
}

// 删除用户的离线消息
void OfflineMsgModel::remove(int userid)
{
    //
    // 先组装 SQL语句，然后调用函数执行SQL语句，成功后获得生成的主键 id
    // 组装sql语句
    char sql[1024] = {0};
    // sprintf 用在这里很方便
    sprintf(sql, "delete from OfflineMessage where userid=%d", userid);
    
    // 定义一个MySQL对象来发射MySQL到数据库内核
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);

    }
}

// 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    //
    // 先组装 SQL语句，然后调用函数执行SQL语句，成功后获得生成的主键 id
    // 组装sql语句
    char sql[1024] = {0};
    // sprintf 用在这里很方便
    sprintf(sql, "select message from OfflineMessage where userid = %d", userid);
    
    vector<string> vec;// return this

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
                // 一行一行地填充
                vec.push_back(row[0]);
            } 

            // 释放mysql资源
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}