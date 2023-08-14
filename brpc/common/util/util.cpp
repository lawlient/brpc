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
    
    
int dayofmonth(int y, int m) {
    static const int ndayofleap[]   = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const int ndayofcommon[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (m < 1 || m > 12) return 0;
    const int* nday = isleapyear(y) ? ndayofleap : ndayofcommon;
    return nday[m-1];
}

bool isleapyear(int y) { return (y % 4 == 0 && y % 100) || (y % 400 == 0); }

int32_t escape(const std::string& in, std::string* out) {
    if(in.empty()) return 0;

    std::vector<char> buf(2 * in.length() + 1);

    char* to         = buf.data();
    char* limit      = to + buf.size();
    const char* from = in.data();
    const char* stop = from + in.length();
                                                                                                    
    int32_t err = 0;
    for(; from < stop; ++from) {
        char escape = 0;
        switch (*from) {
        case    0: escape = '0'; break;
        case '\n': escape = 'n'; break;
        case '\r': escape = 'r'; break;
        case '\\': escape = '\\'; break;
        case '\'': escape = '\''; break;
        case  '"': escape = '"'; break;
        case '\032': escape = 'Z'; break;
        }
                                                                                                    
        if (escape) {
            if (to + 2 > limit) {
                err = 1;   // overflow
                break;
            }
            *to++ = '\\';
            *to++ = escape;
        } else {
            if (to + 1 > limit) {
                err = 1;
                break;
            }
            *to++ = *from;
        }
    }  // end for loop

    // end string
    *to= 0;

    if(err == 0) {
        size_t len = to - buf.data();
        out->assign(buf.data(), len);
    }
    return err;
}

} // namespace util
} // namespace basis
