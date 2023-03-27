#pragma once

#include "mysql.h"

#include "table.pb.h"
#include "jarvis.pb.h"

#include <brpc/server.h>


namespace jarvis {

class JarvisServiceImpl : public jarvis::Jarvis {
public:

    JarvisServiceImpl() {}
    virtual ~JarvisServiceImpl() {}

    virtual void TestQuery(::google::protobuf::RpcController* controller,
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
};



} // namespace jarvis

void mysql_test(::jarvis::financial_response* response);