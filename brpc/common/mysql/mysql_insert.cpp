#include "mysql.h"

#include <butil/logging.h>

#include <sstream>


using namespace google::protobuf;


namespace mysql {


Status MysqlWrapper::InsertRaw(const google::protobuf::Message &raw) {
    Status status;
    if (!connected()) {
        status.code = 1;
        status.msg = "connection is not valid";
        LOG(ERROR) << status.msg;
        return status;
    }

    const auto* desc = raw.GetDescriptor();

    std::stringstream cmd;
    cmd << "INSERT into " << desc->name() << " ";
    int i = 0;
    const auto* field = desc->field(i++);
    cmd << "(`" << field->name() << "`";
    for (; i < desc->field_count(); i++) {
        field = desc->field(i);
        cmd << ", `" << field->name() << "`";
    }
    cmd << ") VALUES (";

    i = 0; 
    field = desc->field(i++);
    append_field(cmd, raw, field);
    for (; i < desc->field_count(); i++) {
        field = desc->field(i);
        cmd << ", ";
        append_field(cmd, raw, field);
    }
    cmd << ")";
    LOG(INFO) << cmd.str();

    execute(cmd.str(), &status);
    return status;
}




} // namespace mysql