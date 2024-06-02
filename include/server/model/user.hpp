#ifndef USER_H
#define USER_H

#include <string>

using namespace std;

// 匹配User表的ORM类，这里是映射字段的（与业务有关），具体的操作在usermodel.cpp中（与业务无关）
// 这样业务就不用 见到 SQL
// 映射到数据库的类
class User {
public:
    // 实现一些方法来对操作对象
    User(int id = -1, string name = "", string pwd = "", string state = "offline")
    {
        this->id = id;
        this->name = name;
        this->password = pwd;
        this->state = state;
    }

    // set 
    void setId(int id) {this->id = id;}
    void setName(string name) {this->name = name;}
    void setPassword(string pwd) {this->password = pwd;}
    void setState(string state) {this->state = state;}

    // get
    int getId() {return this->id;}
    string getName() {return this->name;}
    string getPassword() {return this->password;}
    string getState() {return this->state;}


private:
    int id;
    string name;
    string password;
    string state;

};

#endif