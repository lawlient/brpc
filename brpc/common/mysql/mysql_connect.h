#pragma once

#include "mysql_result.h"
#include "mysql_connect_factory.h"

#include <cppconn/connection.h>

namespace mysql {


class Connection {
public:
    virtual ~Connection() {}

    void Active() { m_active = std::clock(); }

    virtual Result *Execute(const std::string& sql, const google::protobuf::Message *msg) = 0;

    const ConnectFactory &Factory() const { return *_factory; }
    void SetFactory(const ConnectFactory* f) { _factory = f; }

    virtual bool Reconnect() const = 0;

protected:
    Connection() {}

private:
    const ConnectFactory *_factory;
    std::clock_t m_active;  // 激活时间戳
};


class CppConnConnection : public Connection {
public:
    explicit CppConnConnection(sql::Connection *c) { m_conn.reset(c); }

    virtual Result *Execute(const std::string& sql, const google::protobuf::Message *msg) override;

    virtual bool Reconnect() const;

private:
    std::unique_ptr<sql::Connection> m_conn;

};


} //namespace mysql