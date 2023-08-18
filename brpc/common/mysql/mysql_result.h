#pragma once

#include <google/protobuf/message.h>

#include <cppconn/resultset.h>

#include <string>
#include <vector>


namespace mysql {


struct Result {
    int code;
    std::string msg;
    int rows;
    std::vector<google::protobuf::Message *> records;

    Result();
    ~Result();

    void Parse(sql::ResultSet *res, const google::protobuf::Message& proto);
};


    
} // namespace mysql




