#include "util.h"

#include <cstring>

#include <openssl/hmac.h>
#include <openssl/evp.h>



namespace jutil {

std::string nowstring() {
    char buf[50];
    struct tm tm;
    time_t now = time(nullptr);
    localtime_r(&now, &tm);
    strftime(buf, 50, "%F %T", &tm);
    return  std::string(buf);
}

static std::string hmac_sha256(const unsigned char* data, size_t datalen) {
    static const char* KEY = "huba";
    unsigned char md[33];
    unsigned int mdlen = 0;

    HMAC(EVP_sha256(), KEY, strlen(KEY), data, datalen, md, &mdlen);

    char buf[65];
    for (unsigned int i = 0; i < mdlen; i++)
        snprintf(buf + 2*i, 65 - 2*i, "%.2x", md[i]);
    return std::string(buf);
}

std::string token_generator() { 
    const auto& msg = nowstring();
    return hmac_sha256((unsigned char*)msg.c_str(), msg.size());
}

    
} // namespace jutil
