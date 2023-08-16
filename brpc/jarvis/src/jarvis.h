#pragma once

#include "mysql.h"

#include "table.pb.h"
#include "jarvis.pb.h"

#include "util/util.h"

#include <config/config.h>
#include <nlohmann/json.hpp>
#include <brpc/server.h>
#include <google/protobuf/util/json_util.h>


namespace jarvis {

static const int32_t kOwnerUid = 0; /* It is me */


static inline std::shared_ptr<mysql::MysqlWrapper> make_sql_ins() { 
    auto* cfg = basis::text_config::TextConfig::GetInstance();
    const auto& mysql_config = cfg->cfg().mysql();
    mysql::MysqlOption mysql_option;
    mysql_option.url    = mysql_config.url();
    mysql_option.user   = mysql_config.user();
    mysql_option.passwd = mysql_config.passwd();
    mysql_option.schema = mysql_config.schema();
    return std::make_shared<mysql::MysqlWrapper>(mysql_option);
}

template<class R, class P>
bool parse_param_from_http_req(brpc::Controller* cntl, R* response, P* param) {
    google::protobuf::util::JsonParseOptions kOption;
    kOption.ignore_unknown_fields = true;
    const auto& body = cntl->request_attachment().to_string();
    auto s = google::protobuf::util::JsonStringToMessage(body, param, kOption);
    if (!s.ok()) {
        LOG(ERROR) << "parse body: " << body << " fail";
        response->set_status(2);
        response->set_msg(s.message());
        return false;
    }
    return true;
}


class JarvisServiceImpl : public jarvis::Jarvis {
public:

    JarvisServiceImpl() {}
    virtual ~JarvisServiceImpl() {}

    virtual void Login(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::HttpResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void GetFinancialUser(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::HttpResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void AddFinancialUser(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_users_request* request,
                       ::jarvis::financial_users_response* response,
                       ::google::protobuf::Closure* done) override;

    virtual void DelFinancialUser(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_users_request* request,
                       ::jarvis::financial_users_response* response,
                       ::google::protobuf::Closure* done) override;

    virtual void UpdFinancialUser(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_users_request* request,
                       ::jarvis::financial_users_response* response,
                       ::google::protobuf::Closure* done) override;

    virtual void GetFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::HttpResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void AppendFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_request* request,
                       ::jarvis::financial_response* response,
                       ::google::protobuf::Closure* done) override;

    virtual void DeleteFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_request* request,
                       ::jarvis::financial_response* response,
                       ::google::protobuf::Closure* done) override;

    virtual void UpdateFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_request* request,
                       ::jarvis::financial_response* response,
                       ::google::protobuf::Closure* done) override;

    virtual void GetFinancialAsset(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::HttpResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void GetTask(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::TaskResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void AddTask(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::TaskResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void UpdTask(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::TaskResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void DelTask(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::TaskResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void GetOKR(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::OKResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void AddOKR(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::OKResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void UpdOKR(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::OKResponse* response,
                       ::google::protobuf::Closure* done) override;

    virtual void DelOKR(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::OKResponse* response,
                       ::google::protobuf::Closure* done) override;



private:
    static inline void common_cntl_set(::google::protobuf::RpcController* controller) {
        brpc::Controller* cntl = dynamic_cast<brpc::Controller*>(controller);
        if (cntl) {
            auto origin_header = cntl->http_request().GetHeader("Origin");
            std::string origin = origin_header ? *origin_header : "http://localhost:3000";
            cntl->set_always_print_primitive_fields(true);
            cntl->http_response().SetHeader("Access-Control-Allow-Origin", origin);
            cntl->http_response().SetHeader("Access-Control-Allow-Credentials", "true");
            cntl->http_response().SetHeader("Access-Control-Allow-Methods", "GET, POST, PATCH, PUT, DELETE, OPTIONS");
            cntl->http_response().SetHeader("Access-Control-Allow-Headers", "Origin, Content-Type, X-Auth-Token");

            cntl->set_pb_jsonify_empty_array(true);
        }

    }

    void update_token_ttl(); /* 刷新登录token的过期时间，保活用 */
    bool check_login(); /* */
    bool update_user_balance();

private:
};



} // namespace jarvis
