#include "jarvis.h"
#include "util.h"
#include "redis_client.h"


namespace jarvis {


/* 用户数相对有限， 全量查询 */
static void financial_all_users(mysql::MysqlInstance *db, ::jarvis::financial_users_response* response) {
    jarvis::financial_users users;
    mysql::SelectGenerator g(users);
    g.orderby("uid asc");
    g.limit("1000");
    std::unique_ptr<mysql::Result> res(db->Execute(g.sql(), &users));
    for (const auto* msg : res->records) {
        const auto* row = dynamic_cast<const jarvis::financial_users*>(msg);
        if (row == nullptr) {
            LOG(ERROR) << "cast financial users fail";
            continue;
        }
        auto* u = response->add_users();
        u->CopyFrom(*row);
        LOG(DEBUG) << u->ShortDebugString();
    }
}


void JarvisServiceImpl::GetFinancialUser(::google::protobuf::RpcController* controller,
                    const ::jarvis::HttpRequest* request,
                    ::jarvis::HttpResponse* response,
                    ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);

    if (!check_login()) {
        nlohmann::json res;
        res["status"] = 1;
        res["msg"]    = "Please login";
        cntl->response_attachment().append(res.dump());
        return;
    }
    update_token_ttl();

    const auto& uri = cntl->http_request().uri();
    auto optformat = uri.GetQuery("OptionsFormat");
    enum HttpResponseDataFormat formatype(HttpResponseDataFormat::AUTO);
    if (optformat == nullptr) {
        formatype = HttpResponseDataFormat::AUTO;
    } else if (!optformat->compare("OPTIONS")) {
        formatype = HttpResponseDataFormat::OPTIONS;
    } else if (!optformat->compare("MAPPING")) {
        formatype = HttpResponseDataFormat::MAPPING;
    }

    ::jarvis::financial_users_response tmp;
    financial_all_users(_db.get(), &tmp);

    std::ostringstream oss;
    switch (formatype) {
        case HttpResponseDataFormat::OPTIONS: {
            nlohmann::json res;
            res["status"] = 0;
            res["msg"]    = "Success";
            for (const auto& u : tmp.users()) {
                nlohmann::json opt;
                opt["label"] = u.name();
                opt["value"] = u.uid();
                res["data"]["options"].push_back(opt);
            }
            oss << res.dump();
            break;
        }
        case HttpResponseDataFormat::MAPPING: {
            nlohmann::json res;
            res["status"] = 0;
            res["msg"]    = "Success";
            for (const auto& u : tmp.users()) {
                res["data"][std::to_string(u.uid())] = u.name();
            }
            oss << res.dump();
            break;
        }
        case HttpResponseDataFormat::AUTO:
        default: {
            std::string data;
            google::protobuf::util::JsonPrintOptions joption;
            joption.always_print_primitive_fields = true;
            joption.preserve_proto_field_names = true;
            google::protobuf::util::MessageToJsonString(tmp, &data, joption);
            oss << "{\"status\":0, \"msg\":\"Success\", \"data\":" << data << "}";
        }
    }

    cntl->response_attachment().append(oss.str());
    LOG(INFO) << cntl->response_attachment();
}

void JarvisServiceImpl::AddFinancialUser(::google::protobuf::RpcController* controller,
                    const ::jarvis::financial_users_request* request,
                    ::jarvis::financial_users_response* response,
                    ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);

    if (!check_login()) {
        nlohmann::json res;
        res["status"] = 1;
        res["msg"]    = "Please login";
        cntl->response_attachment().append(res.dump());
        return;
    }
    update_token_ttl();

    const auto& user        = request->user();
    const auto* descriptor  = user.descriptor();
    const std::string nows  = basis::util::datetimenow();

    std::ostringstream cmd;
    cmd << "insert into " << descriptor->name() << " ("
           "`uid`, "
           "`name`, "
           "`create_time`, "
           "`update_time`"
           ") values (";
    cmd << user.uid() << ", ";
    cmd << "\"" << user.name() << "\", ";
    cmd << "\"" << nows << "\", ";
    cmd << "\"" << nows << "\"";
    cmd << ")";

    std::unique_ptr<mysql::Result> res(_db->Execute(cmd.str()));

    financial_all_users(_db.get(), response);
    response->set_code(res->rows == 1 ? financial_users_response_rcode_ok : financial_users_response_rcode_dberr);
}

void JarvisServiceImpl::UpdFinancialUser(::google::protobuf::RpcController* controller,
                    const ::jarvis::financial_users_request* request,
                    ::jarvis::financial_users_response* response,
                    ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);

    if (!check_login()) {
        nlohmann::json res;
        res["status"] = 1;
        res["msg"]    = "Please login";
        cntl->response_attachment().append(res.dump());
        return;
    }
    update_token_ttl();

    const auto& user        = request->user();
    const auto* descriptor  = user.descriptor();
    const std::string nows  = basis::util::datetimenow();

    std::ostringstream cmd;
    cmd << "update " << descriptor->name() << " set ";
    cmd << "`name` = " << user.name();
    cmd << "`update_time` = " << nows;
    cmd << " where `uid` = " << user.uid();

    std::unique_ptr<mysql::Result> res(_db->Execute(cmd.str()));

    financial_all_users(_db.get(), response);
    response->set_code(1 == res->rows ? financial_users_response_rcode_ok : financial_users_response_rcode_dberr);
}

void JarvisServiceImpl::DelFinancialUser(::google::protobuf::RpcController* controller,
                    const ::jarvis::financial_users_request* request,
                    ::jarvis::financial_users_response* response,
                    ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);

    if (!check_login()) {
        nlohmann::json res;
        res["status"] = 1;
        res["msg"]    = "Please login";
        cntl->response_attachment().append(res.dump());
        return;
    }
    update_token_ttl();


    const auto& user = request->user();
    std::ostringstream cmd;
    cmd << "delete from financial_users where uid = " << user.uid();
    LOG(INFO) << cmd.str();
    _db->Execute(cmd.str());

    financial_all_users(_db.get(), response);
    response->set_code(financial_users_response_rcode_ok);
    return;
}



    
} // namespace jarvis
