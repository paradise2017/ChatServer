#ifndef GROUP_H
#define GROUP_H

#include "GroupUser.hpp"
#include <string>
#include <vector>
using namespace std;

// User表的ORM类
class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void SetId(int id) { this->id = id; }
    void SetName(string name) { this->name = name; }
    void SetDesc(string desc) { this->desc = desc; }

    int GetId() { return this->id; }
    string GetName() { return this->name; }
    string GetDesc() { return this->desc; }
    vector<GroupUser> &GetUsers() { return this->users; }

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;  //所有组成员
};

#endif