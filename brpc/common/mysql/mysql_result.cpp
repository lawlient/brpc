#include "mysql_result.h"

#include <butil/logging.h>


namespace mysql {


Result::Result()
    : code(0)
    , msg("success")
    , rows(0)
{}

Result::~Result() {
    while (records.size()) {
        delete records.back();
        records.pop_back();
    }
}


void Result::Parse(sql::ResultSet *res, const google::protobuf::Message& proto) {
    const auto* descriptor = proto.GetDescriptor();
    rows = res->getRow();

    try {
        while (res->next()) {
            using namespace google::protobuf;
            google::protobuf::Message* raw = proto.New();
            const auto* refl = raw->GetReflection();

            for (int i = 0; i < descriptor->field_count(); i++) {
                const auto* field = descriptor->field(i);
                const auto& fname = field->name();
                switch(field->type()) {
                    case FieldDescriptor::Type::TYPE_DOUBLE:{ refl->SetDouble(raw, field, res->getDouble(fname));  break; }
                    case FieldDescriptor::Type::TYPE_FLOAT:  { refl->SetFloat(raw, field, res->getDouble(fname));  break; }
                    case FieldDescriptor::Type::TYPE_INT64: { refl->SetInt64(raw, field, res->getInt(fname));  break; }
                    case FieldDescriptor::Type::TYPE_UINT64: { refl->SetUInt64(raw, field, res->getInt(fname));  break; }
                    case FieldDescriptor::Type::TYPE_INT32: { refl->SetInt32(raw, field, res->getInt(fname));  break; }
                    case FieldDescriptor::Type::TYPE_FIXED64: { refl->SetUInt32(raw, field, res->getInt(fname));  break; }
                    case FieldDescriptor::Type::TYPE_FIXED32: { refl->SetInt32(raw, field, res->getInt(fname)); break; }
                    case FieldDescriptor::Type::TYPE_STRING:  { refl->SetString(raw, field, res->getString(fname)); break; }
                    default: break;
                }

            }
            records.emplace_back(std::move(raw));
        }
    } catch(std::exception& e) {
        code = 1;
        msg = e.what();
        LOG(ERROR) << "what: " << e.what();
    }
}







    
} // namespace mysql
