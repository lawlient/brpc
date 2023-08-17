#pragma once

#include "mysql_connect.h"
#include "mysql_connect_factory.h"

#include <bthread/condition_variable.h>

#include <queue>



namespace mysql {


struct PoolOption {
    int initsize;       // 初始链接数
    int maxsize;        // 最大连接数
    int idlesec;        // 空闲回收时长

    PoolOption() : initsize(0), maxsize(0), idlesec(0) {}
};


class ConnectPool {
public:
    ConnectPool(const ConnectFactory* f, const PoolOption* opt = nullptr);
    ~ConnectPool();

    std::shared_ptr<Connection> get();

private: 
    ConnectPool() {}

private:
    const PoolOption* _option;
    const ConnectFactory* _factory;

    bthread::Mutex m_mutex;
    // bthread::ConditionVariable m_cv;
    std::queue<Connection *>  m_pool;
    int nconnections;                   // 当前连接数


};


}  // namespace mysql