#pragma once

#include "mysql_connect_factory.h"
#include "mysql_connect_pool.h"
#include "mysql_connect.h"


#include <google/protobuf/message.h>

#include<memory>
#include<string>

namespace mysql {

class Result;


class MysqlInstance {
public:
    MysqlInstance();
    explicit MysqlInstance(const MysqlOption& opt);
    ~MysqlInstance() {}

    bool Init();

    // msg, 用于select语句返回值的解析
    Result *Execute(const std::string& sql, const google::protobuf::Message* msg=nullptr) const;

private:
    std::unique_ptr<MysqlOption> _option;
    std::unique_ptr<ConnectPool> _pool;
    std::unique_ptr<ConnectFactory> _factory;
};




    
} // namespace mysql
