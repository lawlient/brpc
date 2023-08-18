#include "mysql_connect.h"

#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>


namespace mysql {


static bool begin_with_select(const std::string& s) {
    return s.find("select") != std::string::npos || s.find("SELECT") != std::string::npos;
}

bool CppConnConnection::Reconnect() const {
    if (m_conn->isValid()) return true;

    m_conn->reconnect();
    if (!m_conn->isValid()) {
        return false;
    }
    m_conn->setSchema(Factory().option().schema);
    LOG(INFO) << "connect to schema: " << Factory().option().schema << " success";
    return true;
}

Result *CppConnConnection::Execute(const std::string& sql, const google::protobuf::Message *msg) {
    if (!Reconnect()) {
        Result *s = new Result();
        s->code = 1;
        s->msg = "reconnect to db fail";
        LOG(ERROR) << s->msg;
        return s;
    }

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