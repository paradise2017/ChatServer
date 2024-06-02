#pragma once
#include <string>
#include <vector>
using namespace std;
// 提供离线消息表的接口
class OfflineMessageModel
{

public:
    OfflineMessageModel(){};
    ~OfflineMessageModel(){};

    void insert(int userid, string msg); // 存储用户的离线消息

    void remove(int userid);        // 删除用户的离线消息
    vector<string> query(int userid); // 查询用户的离线消息
private:
};
