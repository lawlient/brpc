#include "mysql.h"
#include "mysql_connect.h"


namespace mysql {


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

MysqlWrapper::~MysqlWrapper() { if (m_conn) delete m_conn; }


bool MysqlWrapper::connected() {
    if (m_conn == nullptr) {
        m_conn = get_driver_instance()->connect(m_option.url, m_option.user, m_option.passwd);
        if (m_conn == nullptr) {
            return false;
        }
    }
    if (m_conn->isValid()) {
        return true;
    }

    m_conn->reconnect();
    if (!m_conn->isValid()) {
        return false;
    }
    LOG(INFO) << "reconnect to mysql";
    m_conn->setSchema(m_option.schema);
    return true;
}

static bool begin_with_select(const std::string& s) {
    return s.find("select") != std::string::npos || s.find("SELECT") != std::string::npos;
}

Result *CppConnConnection::Execute(const std::string& sql, const google::protobuf::Message *msg) {
    std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
    Result *s = new Result();

    try {
        if (begin_with_select(sql)) {
            auto set = stmt->executeQuery(sql);
            if (msg) s->Parse(set, *msg);
        } else {
            s->rows = stmt->executeUpdate(sql);
        }
        LOG(INFO) << "Executing cmd: " << sql << " change rows:" << s->rows;
    } catch (sql::SQLException &e) {
        s->code = e.getErrorCode();  // could be zero
        s->msg  = e.what();
        LOG(ERROR) <<"[" << __FUNCTION__ << "]"
                   << " Code: " << e.getErrorCode()
                   << " Msg: " << e.what()
                   << " SQLState: " << e.getSQLState();
    }
    return s;
}

} // namespace mysql