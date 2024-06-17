#pragma once

#include <mysql/mysql.h>
#include <string>
using namespace std;
#include <chrono>
using namespace chrono;

// 数据库操作类
class MySQL
{
public:
    // 初始化数据库连接
    MySQL();
    // 释放数据库连接资源
    ~MySQL();
    // 连接数据库
    bool connect(string user, string passwd, string dbName, string ip, unsigned short port);
    // 更新操作
    bool update(string sql);
    // 查询操作
    MYSQL_RES *query(string sql);
    // 刷新起始的空闲时间点
    void refreshAliveTime();
    // 计算连接存活的总时长
    long long getAliveTime();
    // 获取连接
    MYSQL* getConnection();
private:
    MYSQL *_conn;
    steady_clock::time_point m_alivetime;//当前时间点
};

