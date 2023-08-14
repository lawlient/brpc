#pragma once

#include <string>
#include <google/protobuf/util/json_util.h>


namespace basis {
namespace util {

std::string datenow();

std::string timenow();

std::string datetimenow();

int dayofmonth(int y, int m);

bool isleapyear(int y);


int32_t escape(const std::string& in, std::string* out);
    
    
} // namespace util
} // namespace basis
