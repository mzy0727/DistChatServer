#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "db.h"
using namespace std;
class ConnectionPool
{
public:
    //单例模式
    static ConnectionPool* getConnectPool();
    ConnectionPool(const ConnectionPool& obj) = delete;
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;
    ~ConnectionPool();

    shared_ptr<MySQL> getConnection();//任务从连接池中获取一个连接
    
private:
    ConnectionPool();//单例模式

    bool parseJsonFile();//解析Json配置文件
    void produceConnection();//生产新的连接
    void recycleConnection();//回收多余连接
    void addConnection();//添加单个连接

    //MysqlConn::connect所需要的参数
    string m_ip;
    string m_user;
    string m_passwd;
    string m_dbName;
    unsigned short m_port;

    //连接池的参数
    int m_minSize;//最小连接数
    int m_maxSize;//最大连接数
    int m_timeout;//超时等待时间
    int m_maxIdleTime;//待回收线程的超时时间

    queue<MySQL*> m_connectionQ;//任务队列
    mutex m_mutexQ;//互斥锁
    condition_variable m_cond;//条件变量
};
