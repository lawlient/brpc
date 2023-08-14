#include "mysql.h"

#include <butil/logging.h>

#include <sstream>

using namespace google::protobuf;


namespace mysql {

Status MysqlWrapper::UpdateRaw(const google::protobuf::Message &raw, const std::string& where) {
    Status status;
    if (!connected()) {
        status.code = 2;
        status.msg = "connection is not valid";
        LOG(ERROR) << status.msg;
        return status;
    }

    const auto* desc = raw.GetDescriptor();

    std::stringstream cmd;
    cmd << "UPDATE " << desc->name() << " ";
    int i = 0;
    const auto* field = desc->field(i++);
    cmd << "SET `" << field->name() << "` = ";
    append_field(cmd, raw, field);

    for (; i < desc->field_count(); i++) {
        field = desc->field(i);
        cmd << ", `" << field->name() << "` = ";
        append_field(cmd, raw, field);
    }
    cmd << " WHERE " << where;

    LOG(INFO) << cmd.str();

    execute(cmd.str(), &status);
    return status;
}




} // namespace mysql