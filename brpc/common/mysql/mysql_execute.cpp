#include "mysql.h"



bool MysqlWrapper::Execute(const std::string& cmd) {
    if (!m_initialized) return false;
    if (!check_connection()) {
        LOG(ERROR) << "connection is not valid";
        return false;
    }

    bool success = false;
    std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());

    try {
        success = stmt->execute(cmd);
        LOG(INFO) << "Executing cmd: " << cmd << (success ? " success" : " failure");
    } catch (sql::SQLException &e) {
        LOG(ERROR) <<"[" << __FUNCTION__ << "]"
                   << " Code: " << e.getErrorCode()
                   << " Msg: " << e.what()
                   << " SQLState: " << e.getSQLState();
    }
    return success;
}
