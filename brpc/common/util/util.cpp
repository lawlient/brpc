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
    
    
} // namespace util
} // namespace basis
