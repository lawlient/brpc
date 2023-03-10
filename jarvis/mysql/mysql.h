#pragma once


#include <google/protobuf/message.h>
/*
 *   Include directly the different
 *   headers from cppconn/ and mysql_driver.h + mysql_util.h
 *   (and mysql_connection.h). This will reduce your build time!
 */
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

struct MysqlOption {
    std::string url;
    std::string user;
    std::string passwd;
    std::string schema;
};

class MysqlWrapper {
public:
    explicit MysqlWrapper(const MysqlOption& option);
    ~MysqlWrapper();

    void Execute(const std::string& cmd) const;

    sql::ResultSet *SelectAll(const google::protobuf::Message &meta,
                              const std::string where,
                              const std::string orderby,
                              const std::string groupby,
                              const std::string limit) const;
    int Fprintf(int fd, const google::protobuf::Message& meta, sql::ResultSet *res) const;
    int Parse(sql::ResultSet *res, google::protobuf::Message* meta, std::vector<google::protobuf::Message*>& msgs) const;

private:                            
    MysqlWrapper();

private:
    bool m_initialized;

    const MysqlOption& m_option;
    sql::Driver *m_driver;
    sql::Connection *m_conn;
};


extern std::shared_ptr<MysqlWrapper> mysql_instance;






