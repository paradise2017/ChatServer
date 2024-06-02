#include "db.h"
// 初始化数据库连接

MySQL::MySQL()
{
    conn_ = mysql_init(nullptr);
}
// 释放数据库连接资源 这里用UserModel示例，通过UserModel如何对业务层封装底层数据库的操作。代码示例如下：
MySQL::~MySQL()
{
    if (conn_ != nullptr)
        mysql_close(conn_);
}
// 连接数据库
bool MySQL::connect()
{
    MYSQL *p = mysql_real_connect(conn_, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    // c+,cPP默认的编码字符ASCII,如何不设置，乱码
    if (p != nullptr)
    {
        mysql_query(conn_, "set names gbk");
         LOG_INFO << " connect mysql success!";
    }else{
         LOG_INFO << " connect mysql error!";
    }
    return p;
}
// 更新操作
bool MySQL::update(string sql)
{
    if (mysql_query(conn_, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "更新失败!";
        return false;
    }
    return true;
}
// 查询操作
MYSQL_RES *MySQL::query(string sql)
{
    if (mysql_query(conn_, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":" << sql << "查询失败!";
        return nullptr;
    }
    return mysql_use_result(conn_);
}

    //获取连接
MYSQL* MySQL::GetConnection(){
    return conn_;
}