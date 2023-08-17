#include "mysql_connect_pool.h"


namespace mysql {

#define LOCK std::lock_guard<bthread_mutex_t> guard(*m_mutex.native_handler());

static PoolOption kPoolOption;

ConnectPool::ConnectPool(const ConnectFactory* f, const PoolOption* opt) 
    : _factory(f) 
{
    kPoolOption.initsize = 2;
    kPoolOption.maxsize = 4;
    kPoolOption.idlesec = 300;
    if (opt) {
        _option = new PoolOption(*opt);
    } else {
        _option = &kPoolOption;
    }
    for (int i = 0; i < _option->initsize; i++) {
        m_pool.push(_factory->make());
    }
    nconnections = _option->initsize;
}

ConnectPool::~ConnectPool() {
    if (_option != &kPoolOption) delete _option;

    while (m_pool.size()) {
        delete m_pool.front();
        m_pool.pop();
    }
}

std::shared_ptr<Connection> ConnectPool::get() {
    auto collect = [&](Connection* c) {
        c->Active();

        LOCK
        m_pool.push(c);
    };

    LOCK
    if (m_pool.empty()) {
        if (nconnections < _option->maxsize) {
            auto conn = _factory->make();
            if (!conn) return nullptr;
            auto c = std::shared_ptr<Connection>(conn, collect);
            nconnections++;
            return c;
        }
        return nullptr;
    }

    auto r = std::shared_ptr<Connection>(m_pool.front(), collect);
    m_pool.pop();
    return r;
}


}