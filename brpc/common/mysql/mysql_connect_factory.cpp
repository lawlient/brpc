#include "mysql_connect_factory.h"
#include "mysql_connect.h"

namespace mysql {


CppConnConnectFactory::CppConnConnectFactory(const MysqlOption& opt)
    : ConnectFactory(opt) 
{
    m_driver = get_driver_instance();
}

Connection *CppConnConnectFactory::create() const {
    const auto& koption = option();
    sql::Connection *conn = m_driver->connect(koption.url, koption.user, koption.passwd);
    if (!conn) return nullptr;
    conn->setSchema(koption.schema);
    Connection *c = new CppConnConnection(conn);
    c->SetFactory(this);
    c->Active();
    return c;
}


    
} // namespace mysql









