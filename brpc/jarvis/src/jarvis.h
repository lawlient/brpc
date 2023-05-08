#pragma once

#include "mysql.h"

#include "table.pb.h"
#include "jarvis.pb.h"

#include <config/config.h>
#include <nlohmann/json.hpp>
#include <brpc/server.h>
#include <google/protobuf/util/json_util.h>


namespace jarvis {

static const int32_t kOwnerUid = 0; /* It is me */

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
        }

    }

    void update_token_ttl(); /* 刷新登录token的过期时间，保活用 */
    bool check_login(); /* */
    bool update_user_balance();


private:
};



} // namespace jarvis
