#include "mysql.h"

#include <butil/logging.h>

#include <sstream>


using namespace google::protobuf;

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

    std::unique_ptr<sql::Statement> stmt(m_conn->createStatement());
    return stmt->executeQuery(cmd.str());
}