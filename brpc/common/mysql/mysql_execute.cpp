#include "mysql.h"

namespace mysql {


bool MysqlWrapper::Execute(const std::string& cmd) {
    if (!m_initialized) return false;
    if (!connected()) {
        LOG(ERROR) << "connection is not valid";
        return false;
    }

    Status s;
    execute(cmd, &s);
    return s.rows > 0;
}

sql::ResultSet *MysqlWrapper::ExecuteQuery(const std::string& cmd) {
    if (!m_initialized) return nullptr;
    if (!connected()) {
        LOG(ERROR) << "connection is not valid";
        return nullptr;
    }

    std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
    sql::ResultSet *res = nullptr;

    try {
        res = stmt->executeQuery(cmd);
    } catch (sql::SQLException &e) {
        LOG(ERROR) <<"[" << __FUNCTION__ << "]"
                   << " Code: " << e.getErrorCode()
                   << " Msg: " << e.what()
                   << " SQLState: " << e.getSQLState();
    }
    return res;
}


void MysqlWrapper::execute(const std::string& cmd, Status* s) {
    std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());

    try {
        s->rows = stmt->executeUpdate(cmd);
        LOG(INFO) << "Executing cmd: " << cmd << " change rows:" << s->rows;
    } catch (sql::SQLException &e) {
        s->code = e.getErrorCode();  // could be zero
        s->msg  = e.what();
        LOG(ERROR) <<"[" << __FUNCTION__ << "]"
                   << " Code: " << e.getErrorCode()
                   << " Msg: " << e.what()
                   << " SQLState: " << e.getSQLState();
    }
}


} // namespace mysql