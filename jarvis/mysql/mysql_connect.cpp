#include "mysql.h"

#include <brpc/server.h>
#include <gflags/gflags.h>


std::shared_ptr<MysqlWrapper> mysql_instance = nullptr;



MysqlWrapper::MysqlWrapper(const MysqlOption& option) : m_option(option)
{ 
    m_driver = get_driver_instance();
    /* Create a connection */
    LOG(INFO) << "url:" << option.url << " user:" << option.user;
    m_conn = m_driver->connect(option.url, option.user, option.passwd);
    if (!m_conn) {
        LOG(ERROR) << "url:" << option.url << " user:" << option.user;
        m_initialized = false;
        return;
    }

    /* Connect to the MySQL `schema` database */
    m_conn->setSchema(option.schema);
    m_initialized = true;
}


void MysqlWrapper::Execute(const std::string& cmd) const {
    if (!m_initialized) return;

    try {
        std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
        stmt->execute(cmd);
    } catch (sql::SQLException &e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }
}

MysqlWrapper::~MysqlWrapper() { if (m_conn) delete m_conn; }



