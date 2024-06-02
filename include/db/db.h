#pragma once
#include <mysql/mysql.h>
#include <string>
#include <muduo/base/Logging.h>
using namespace std;
static string server = "127.0.0.1";
static string user = "root";
static string password = "Apple@1234";
static string dbname = "chat";

// 数据库操作类
class MySQL
{
public:
    // 初始化数据库连接
    MySQL();
    // 释放数据库连接资源
    ~MySQL();
    // 连接数据库
    bool connect();
    // 更新操作
    bool update(string sql);
    // 查询操作
    MYSQL_RES *query(string sql);
    MYSQL* GetConnection();
private:
    MYSQL *conn_;
};
