#include "UserModel.hpp"
#include "db.h"
#include <iostream>
using namespace std;
bool UserModel::insert(User &user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name,password,state) values('%s','%s','%s')", user.GetName().c_str(), user.GetPassWord().c_str(), user.GetState().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取用户成功的主键ID
            user.SetId(mysql_insert_id(mysql.GetConnection()));

            return true;
        }
    }
    return false;
}

User UserModel::query(int id)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.SetId(atoi(row[0]));
                user.SetName(row[1]);
                user.SetPassWord(row[2]);
                user.SetState(row[3]); // 0 1 2 3 对应字段
                mysql_free_result(res);
                return user;
            }
        }
    }

    return User();
}

bool UserModel::UpdateState(User user)
{
    // 组装sql语句
    char sql[1024] = {0};

    sprintf(sql, "update user set state = '%s' where id = %d", user.GetState().c_str(), user.GetId());
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

void UserModel::ResetState()
{
    // 组装sql语句
    char sql[1024] = "update user set state = 'offline' where state = 'online'";

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}