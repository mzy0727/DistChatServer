#include "OfflineMsgModel.h"
#include "db.h"
#include "ConnectionPool.h"
 // 存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg){
    char sql[1024] = {0};
    sprintf(sql,"insert into offlinemessage values(%d,'%s')",userid,msg.c_str());

   // MySQL mysql;
    ConnectionPool* pool = ConnectionPool::getConnectPool();
    shared_ptr<MySQL> conn = pool->getConnection();
    if(conn){
       conn->update(sql);
    }
}

// 删除用户的离线消息
void OfflineMsgModel::remove(int userid){
    char sql[1024] = {0};
    sprintf(sql,"delete from offlinemessage where userid = %d",userid);

    //MySQL mysql;
      ConnectionPool* pool = ConnectionPool::getConnectPool();
    shared_ptr<MySQL> conn = pool->getConnection();
    if(conn){
        conn->update(sql);
    }
}

// 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid){
    char sql[1024] = {0};
    sprintf(sql,"select message from offlinemessage where userid = %d",userid);
   // MySQL mysql;
    vector<string> vec;
      ConnectionPool* pool = ConnectionPool::getConnectPool();
    shared_ptr<MySQL> conn = pool->getConnection();
    if(conn){
        MYSQL_RES *res = conn->query(sql);
        if(res != nullptr){
            // 把userid用户的所有离线消息放入vec中返回
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}