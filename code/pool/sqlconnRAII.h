#ifndef SQLCONNRAII_H
#define SQLCONNRAII_H


#include "sqlconnpool.h"

class sqlConnRAII {
public:
    sqlConnRAII(MYSQL** sql, SqlConnPool *connpool) {
        assert(connpool);
        *sql = connpool->getConn();
        sql_ = *sql;
        connpool_ = connpool;
    }
    
    ~sqlConnRAII() {
        if(sql_) { connpool_->freeConn(sql_); }
    }
    
private:
    MYSQL *sql_;
    SqlConnPool* connpool_;
};

#endif
