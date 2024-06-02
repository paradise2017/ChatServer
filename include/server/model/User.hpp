#pragma once
#include <string>
using namespace std;

//匹配User表的ORM类  业务 没有具体的sql
class User
{

public:
    User(int id = -1, string name = "", string pwd = "", string state = "offline")
    {
        id_ = id;
        name_ = name;
        password_ = pwd;
        state_ = state;
    }
    ~User(){}

    void SetId(int id)
    {
        id_ = id;
    }
    void SetName(string name)
    {
        name_ = name;
    }

    void SetPassWord(string pwd)
    {
        password_ = pwd;
    }
    void SetState(string state)
    {
        state_ = state;
    }

    int GetId()
    {
        return id_;
    }
    string GetName()
    {
        return name_;
    }

    string GetPassWord()
    {
        return password_;
    }
    string GetState()
    {
        return state_;
    }

private:
    int id_;
    string name_;
    string password_;
    string state_;
};
