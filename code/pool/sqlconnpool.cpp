#include "sqlconnpool.h"

SqlConnPool::SqlConnPool() {
    useCount = 0;
    freeCount = 0;
    MAX_CONNNUMS = 0;
}

SqlConnPool::~SqlConnPool(){
    closePool();
}

SqlConnPool* SqlConnPool::Instance() {
    static SqlConnPool connPool;
    return &connPool;
}

void SqlConnPool::init(const char* host, int port,
            const char* user,const char* pwd, 
            const char* dbName, int connSize = 10){
    assert(connSize > 0);

    for(int i = 0; i < connSize; i++) {
        MYSQL *sql = nullptr;
        sql = mysql_init(sql);
        if(!sql){
            LOG_ERROR("MySql init error!");
            assert(sql);
        }

        sql = mysql_real_connect(sql, host, user,  pwd, dbName, port, nullptr, 0);
        if (!sql) {
            LOG_ERROR("MySql Connect error!");
        }
        connQue.push(sql);
    }

    MAX_CONNNUMS = connSize;
    sem_init(&m_sem, 0, MAX_CONNNUMS);
}

MYSQL* SqlConnPool::getConn(){
    MYSQL *sql = nullptr;
    if(connQue.empty()) {
        LOG_WARN("SqlConnPool is busy");
        return nullptr;
    }

    sem_wait(&m_sem);
    {
        std::lock_guard<std::mutex> locker(m_mtx);
        sql = connQue.front();
        connQue.pop();
    }
    return sql;
}

int SqlConnPool::GetFreeConnCount(){
    std::lock_guard<std::mutex> locker(m_mtx);
    return connQue.size();
}

void SqlConnPool::freeConn(MYSQL *sql){
    assert(sql);
    std::lock_guard<std::mutex> locker(m_mtx);
    connQue.push(sql);
    sem_post(&m_sem);
}

void SqlConnPool::closePool(){
    std::lock_guard<std::mutex> locker(m_mtx);
    while(!connQue.empty()) {
        auto tmp = connQue.front();
        connQue.pop();
        mysql_close(tmp);
    }
    mysql_library_end();
}