



#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include "../log/log.h"
#include <assert.h>



class SqlConnPool {
public:
    static SqlConnPool* Instance();
    void init(const char* host, int port,
              const char* user,const char* pwd, 
              const char* dbName, int connSize);
    MYSQL* getConn();
    void freeConn(MYSQL *sql);
    void closePool();
    int GetFreeConnCount();

private:
    SqlConnPool();
    ~SqlConnPool();

  

    int MAX_CONNNUMS;
    int useCount;
    int freeCount;

    std::queue<MYSQL*> connQue;
    std::mutex m_mtx;
    sem_t m_sem;
};


#endif