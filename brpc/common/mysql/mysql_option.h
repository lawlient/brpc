#pragma once

#include <string>

#include <butil/logging.h>

namespace mysql {

struct MysqlOption {
    std::string url;
    std::string user;
    std::string passwd;
    std::string schema;
};



} // namespace mysql
