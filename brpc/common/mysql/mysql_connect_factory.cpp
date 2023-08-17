#include "mysql_connect_factory.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

namespace mysql {


CppConnConnectFactory::CppConnConnectFactory(const MysqlOption& opt) : koption(opt) {
    m_driver = get_driver_instance();
}

Connection *CppConnConnectFactory::make() const {
    sql::Connection *conn = m_driver->connect(koption.url, koption.user, koption.passwd);
    if (!conn) return nullptr;
    conn->setSchema(koption.schema);
    Connection *c = new CppConnConnection(conn);
    c->Active();
    return c;
}




    
} // namespace mysql









