#include "mysql.h"



bool MysqlWrapper::Execute(const std::string& cmd) {
    if (!m_initialized) return false;
    if (!connected()) {
        LOG(ERROR) << "connection is not valid";
        return false;
    }

    return execute(cmd) > 0;
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


int MysqlWrapper::execute(const std::string& cmd) {
    std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
    int change_rows = 0;

    try {
        change_rows = stmt->executeUpdate(cmd);
        LOG(INFO) << "Executing cmd: " << cmd << " change rows:" << change_rows;
    } catch (sql::SQLException &e) {
        LOG(ERROR) <<"[" << __FUNCTION__ << "]"
                   << " Code: " << e.getErrorCode()
                   << " Msg: " << e.what()
                   << " SQLState: " << e.getSQLState();
    }
    return change_rows;
}