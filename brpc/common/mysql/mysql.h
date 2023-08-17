#pragma once

#include <butil/logging.h>
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


namespace mysql {

struct MysqlOption {
    std::string url;
    std::string user;
    std::string passwd;
    std::string schema;
};

struct Status {
    int code;
    std::string msg;
    int rows;

    Status() : code(0), msg("success"), rows(0) {}
};

class MysqlWrapper {
public:
    explicit MysqlWrapper(const MysqlOption& option);
    ~MysqlWrapper();

    bool Execute(const std::string& cmd);
    sql::ResultSet *ExecuteQuery(const std::string& cmd);

    sql::ResultSet *SelectAll(const google::protobuf::Message &meta,
                              const std::string where,
                              const std::string orderby,
                              const std::string groupby,
                              const std::string limit);
    int Fprintf(int fd, const google::protobuf::Message& meta, sql::ResultSet *res) const;
    int Parse(sql::ResultSet *res, google::protobuf::Message* meta, std::vector<google::protobuf::Message*>& msgs) const;

    Status InsertRaw(const google::protobuf::Message &raw);
    Status UpdateRaw(const google::protobuf::Message &raw, const std::string& where);

private:
    MysqlWrapper();


    bool connected();
    void execute(const std::string& cmd, Status* s);

    static std::ostream& append_field(std::ostream& cmd, const google::protobuf::Message &raw, const google::protobuf::FieldDescriptor* field);

private:
    bool m_initialized;

    const MysqlOption& m_option;
    sql::Driver *m_driver;
    sql::Connection *m_conn;
};

extern std::shared_ptr<MysqlWrapper> mysql_instance;






} // namespace mysql
