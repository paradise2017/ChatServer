#pragma once
// server和client公共文件

enum EnMsgType
{
    LOGIN_MSG = 1,  // 登录消息
    LOGIN_MSG_ACK,  // 登录响应消息
    REG_MSG,        // 注册消息
    REG_MSG_ACK,    // 注册响应消息
    ONE_CHAT_MSG,   // 聊天消息
    ADD_FRIEND_MSG, // 添加好友消息
    GROUP_CHAT_MSG, // 群组聊天
    CREATE_GROUP_MSG, //创建群组消息
    ADD_GROUP_MSG, //添加群组消息
    LOGINOUT_MSG,   // 退出登录
};
