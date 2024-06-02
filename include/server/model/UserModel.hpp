#pragma once
#include "User.hpp"
// User表的数据操作类
// orm层  数据库的表增删改查
class UserModel
{

public:
    UserModel(/* args */){};
    ~UserModel(){};

    bool insert(User &user); // user表增加

    User query(int id);          // 用户号码，查询用户信息
    bool UpdateState(User user); // 更新用户的状态信息
    void ResetState();           // 重置用户的状态信息
private:
    /* data */
};
