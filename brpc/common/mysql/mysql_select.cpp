#include "mysql.h"

#include <butil/logging.h>

#include <sstream>


using namespace google::protobuf;


namespace mysql {

sql::ResultSet *MysqlWrapper::SelectAll(const google::protobuf::Message &meta,
                                        const std::string where,
                                        const std::string orderby,
                                        const std::string groupby,
                                        const std::string limit) {
    if (!connected()) {
        LOG(ERROR) << "connection is not valid";
        return nullptr;
    }

    const auto *descriptor = meta.GetDescriptor();
    assert(descriptor);

    const std::string& table = descriptor->name();
    std::ostringstream cmd;
    cmd << "select * from " << table;
    if (!where.empty())   cmd << " where " << where;
    if (!orderby.empty()) cmd << " order by " << orderby;
    if (!groupby.empty()) cmd << " group by " << groupby;
    if (!limit.empty())   cmd << " limit " << limit;
    LOG(INFO) << "cmd: " << cmd.str();

    sql::ResultSet* res = nullptr;
    try {
        std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
        res = stmt->executeQuery(cmd.str());
    } catch (sql::SQLException &e) {
        LOG(ERROR) <<"[" << __FUNCTION__ << "]"
                   << " Code: " << e.getErrorCode()
                   << " Msg: " << e.what()
                   << " SQLState: " << e.getSQLState();
    }
    return res;
}


} // namespace mysql