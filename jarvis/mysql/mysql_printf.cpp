#include "mysql.h"


using namespace google::protobuf;


int MysqlWrapper::Fprintf(int fd, const google::protobuf::Message& meta, sql::ResultSet *res) const {
    const auto* descriptor = meta.GetDescriptor();
    assert(descriptor);
    // print header
    for (int i = 0; i < descriptor->field_count(); i++) {
        const auto* field = descriptor->field(i);
        switch(field->type()) {
            case FieldDescriptor::Type::TYPE_DOUBLE:
            case FieldDescriptor::Type::TYPE_FLOAT: { dprintf(fd, "%7s", field->name().c_str()); break; }
            case FieldDescriptor::Type::TYPE_INT64: 
            case FieldDescriptor::Type::TYPE_UINT64:
            case FieldDescriptor::Type::TYPE_INT32:
            case FieldDescriptor::Type::TYPE_FIXED64:
            case FieldDescriptor::Type::TYPE_FIXED32: { dprintf(fd, "%6s", field->name().c_str()); break; }
            case FieldDescriptor::Type::TYPE_STRING: { dprintf(fd, "%30s", field->name().c_str()); break; }
            default: break;
        }
    }
    dprintf(fd, "\n");

    // print rows
    while (res->next()) {
    for (int i = 0; i < descriptor->field_count(); i++) {
        const auto* field = descriptor->field(i);
        const auto& fname = field->name();
        switch(field->type()) {
            case FieldDescriptor::Type::TYPE_DOUBLE:
            case FieldDescriptor::Type::TYPE_FLOAT: { dprintf(fd, "%7.1Lf", res->getDouble(fname)); break; }
            case FieldDescriptor::Type::TYPE_INT64: 
            case FieldDescriptor::Type::TYPE_UINT64:
            case FieldDescriptor::Type::TYPE_INT32:
            case FieldDescriptor::Type::TYPE_FIXED64:
            case FieldDescriptor::Type::TYPE_FIXED32: { dprintf(fd, "%6d", res->getInt(i+1)); break; }
            case FieldDescriptor::Type::TYPE_STRING: { dprintf(fd, "%30s", res->getString(i+1).c_str()); break; }
            default: break;
        }
    }
    dprintf(fd, "\n");
    }
    return 0;
}


int MysqlWrapper::Parse(sql::ResultSet *res, google::protobuf::Message* msg, std::vector<google::protobuf::Message*>& msgs) const {
    const auto* descriptor = msg->GetDescriptor();
    const auto* reflection = msg->GetReflection();
    assert(descriptor);
    assert(reflection);

    while (res->next()) {
    for (int i = 0; i < descriptor->field_count(); i++) {
        const auto* field = descriptor->field(i);
        const auto& fname = field->name();
        switch(field->type()) {
            case FieldDescriptor::Type::TYPE_DOUBLE:
            case FieldDescriptor::Type::TYPE_FLOAT:  { reflection->SetFloat(msg, field, res->getDouble(fname));  break; }
            case FieldDescriptor::Type::TYPE_INT64: 
            case FieldDescriptor::Type::TYPE_UINT64:
            case FieldDescriptor::Type::TYPE_INT32:
            case FieldDescriptor::Type::TYPE_FIXED64:
            case FieldDescriptor::Type::TYPE_FIXED32: { reflection->SetInt32(msg, field, res->getInt(fname)); break; }
            case FieldDescriptor::Type::TYPE_STRING:  { reflection->SetString(msg, field, res->getString(fname)); break; }
            default: break;
        }
    }
    google::protobuf::Message* nmsg = msg->New();
    nmsg->CopyFrom(*msg);
    msgs.emplace_back(nmsg);
    }
    return 0;
}