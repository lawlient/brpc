#pragma once

#include "mysql.h"

#include "mysql_connect.h"



namespace mysql {


class ConnectFactory {
public: 
    virtual Connection *make() const = 0;
};



class CppConnConnectFactory : public ConnectFactory {
public:
    CppConnConnectFactory(const MysqlOption& opt);

    virtual Connection *make() const override;


private:
    const MysqlOption& koption;
    sql::Driver *m_driver;      // cppconn 的驱动
};



}