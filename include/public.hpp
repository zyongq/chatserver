# ifndef PUBLIC_H
# define PUBLIC_H

// server 和 client 的公共的头文件

// message 的类型
enum EnMsgType {
    LOGIN_MSG = 1, // login message
    LOGIN_MSG_ACK, // 登录响应
    LOGINOUT_MSG, // 注销消息
    REG_MSG ,  // register message
    REG_MSG_ACK, // 注册响应消息
    ONE_CHAT_MSG,   // 单人聊天
    ADD_FRIEND_MSG, // 添加好友消息

    CREATE_GROUP_MSG,// 创建群组
    ADD_GROUP_MSG, // 加入群组
    GROUP_CHAT_MSG // 群聊天
    /*
    发送的信息的json:
    msgid : int
    id : my id -int
    from : my name - "zhang san"
    to : your id - int
    msg : "abcd..."
{"msgid":1,"id":2,"password":"123456"} 登录lisi
{"msgid":1,"id":1,"password":"123456"} 登录 zhang san
{"msgid":5,"id":2,"from":"li si","to":1,"msg":"caonima"}
{"msgid":5,"id":1,"from":"zhang san","to":2,"msg":"caonima"}
{"msgid":6,"id":1,"friendid":2} 张三添加李四为好友
    服务端收到消息之后：对方在线则立即发送，不在线则等上线再发送
    */
};

# endif