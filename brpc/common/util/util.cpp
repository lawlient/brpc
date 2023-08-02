#include "util.h"



namespace basis {
namespace util {
        
static inline std::string now(const char* fmt) {
    char buf[50];
    struct tm tm;
    time_t now = time(nullptr);
    localtime_r(&now, &tm);
    strftime(buf, 50, fmt, &tm);
    return  std::string(buf);
}

std::string datenow() { return now("%F"); }

std::string timenow() { return now("%T"); }

std::string datetimenow() { return now("%F %T"); }
    
    
std::string pb2json(const google::protobuf::Message& m) {
    google::protobuf::util::JsonPrintOptions joption;
    joption.always_print_primitive_fields = true;
    joption.preserve_proto_field_names = true;
    std::string data;
    google::protobuf::util::MessageToJsonString(m, &data, joption);
    return data;
}

} // namespace util
} // namespace basis
