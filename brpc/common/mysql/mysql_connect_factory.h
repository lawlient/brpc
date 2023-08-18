#pragma once

#include "mysql_option.h"

#include <cppconn/driver.h>


namespace mysql {

class Connection;


class ConnectFactory {
public:
    ConnectFactory() = delete;
    explicit ConnectFactory(const MysqlOption& opt) : _option(opt) {}
    virtual Connection *create() const = 0;

    const MysqlOption& option() const { return _option; }

private:
    const MysqlOption& _option;
};



class CppConnConnectFactory : public ConnectFactory {
public:
    CppConnConnectFactory(const MysqlOption& opt);

    virtual Connection *create() const override;

private:
    sql::Driver *m_driver;      // cppconn 的驱动
};



}