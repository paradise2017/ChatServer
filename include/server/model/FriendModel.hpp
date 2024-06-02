#pragma once
#include <vector>
#include "User.hpp"
using namespace std;
//提供好友信息的操作接口方法
class FriendModel
{
public:
    void insert(int userid,int friendid);       //添加好友关系

    vector<User> query(int userid);           //返回用户好友列表  通过用户id 找到用户列表

private:
    /* data */
public:
    FriendModel(){};
    ~FriendModel(){};
};


