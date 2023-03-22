#include "jarvis.h"
#include "mysql.h"

#include <google/protobuf/util/json_util.h>


#define BLOCK_OPTIONS_REQUEST   do {                                        \
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);    \
    auto* method = cntl->method();                                          \
    if (method && !method->name().compare("OPTIONS")) {                     \
        LOG(INFO) << "Preflight request , discard it.";                     \
        return ;                                                            \
    }                                                                       \
} while (0);

using namespace google::protobuf;

namespace jarvis {

static inline void common_cntl_set(::google::protobuf::RpcController* controller) {
    brpc::Controller* cntl = dynamic_cast<brpc::Controller*>(controller);
    if (cntl) {
        cntl->set_always_print_primitive_fields(true);
        cntl->http_response().SetHeader("Access-Control-Allow-Origin", "http://wsl:333");
        cntl->http_response().SetHeader("Access-Control-Allow-Credentials", "true");
        cntl->http_response().SetHeader("Access-Control-Allow-Methods", "GET, POST, PATCH, PUT, DELETE, OPTIONS");
        cntl->http_response().SetHeader("Access-Control-Allow-Headers", "Origin, Content-Type, X-Auth-Token");
    }
}

static inline std::string nowstring() {
    char buf[50];
    struct tm tm;
    time_t now = time(nullptr);
    localtime_r(&now, &tm);
    strftime(buf, 50, "%F %T", &tm);
    return  std::string(buf);
}

void financial_all_users(::jarvis::financial_users_response* response) {
    std::unique_ptr<sql::ResultSet> res;
    jarvis::financial_users users;
    res.reset(mysql_instance->SelectAll(users, "", "`uid` desc", "", "100"));
    if (!res) return;

    std::vector<google::protobuf::Message*> msgs;
    mysql_instance->Parse(res.get(), &users, msgs);

    for (const auto* msg : msgs) {
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

void financial_all_records(::jarvis::financial_response* response) {
    std::unique_ptr<sql::ResultSet> res;
    jarvis::financial_records financial;
    res.reset(mysql_instance->SelectAll(financial, "", "`when` asc", "", "100"));
    if (!res) return;

    std::vector<google::protobuf::Message*> msgs;
    mysql_instance->Parse(res.get(), &financial, msgs);

    for (const auto* msg : msgs) {
        const auto* row = dynamic_cast<const jarvis::financial_records*>(msg);
        if (row == nullptr) {
            LOG(ERROR) << "cast financial records fail";
            continue;
        }
        auto* r = response->add_records();
        r->CopyFrom(*row);
        LOG(DEBUG) << r->ShortDebugString();
    }
}

void JarvisServiceImpl::TestQuery(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::HttpResponse* response,
                       ::google::protobuf::Closure* done)  {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    BLOCK_OPTIONS_REQUEST

    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    LOG(INFO) << cntl->request_attachment();

    ::jarvis::financial_response tmp;
    financial_all_records(&tmp);

    std::string data;
    google::protobuf::util::JsonPrintOptions option;
    option.always_print_primitive_fields = true;
    option.preserve_proto_field_names = true;
    google::protobuf::util::MessageToJsonString(tmp, &data, option);

    std::ostringstream oss;
    oss << "{\"status\":0, \"msg\":\"Success\", \"data\":" << data << "}";

    cntl->response_attachment().append(oss.str());

    LOG(INFO) << cntl->response_attachment();
}

void JarvisServiceImpl::AddFinancialUser(::google::protobuf::RpcController* controller,
                    const ::jarvis::financial_users_request* request,
                    ::jarvis::financial_users_response* response,
                    ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    BLOCK_OPTIONS_REQUEST

    const auto& user        = request->user();
    const auto* descriptor  = user.descriptor();
    const std::string nows  = nowstring();

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

    bool suc = mysql_instance->Execute(cmd.str());

    financial_all_users(response);
    response->set_code(suc ? financial_users_response_rcode_ok : financial_users_response_rcode_dberr);
}

void JarvisServiceImpl::DelFinancialUser(::google::protobuf::RpcController* controller,
                    const ::jarvis::financial_users_request* request,
                    ::jarvis::financial_users_response* response,
                    ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    BLOCK_OPTIONS_REQUEST

    const auto& user = request->user();
    std::ostringstream cmd;
    cmd << "delete from financial_users where uid = " << user.uid();
    LOG(INFO) << cmd.str();
    mysql_instance->Execute(cmd.str());

    financial_all_users(response);
    response->set_code(financial_users_response_rcode_ok);
    return;
}


void JarvisServiceImpl::AppendFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_request* request,
                       ::jarvis::financial_response* response,
                       ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    BLOCK_OPTIONS_REQUEST

    const auto& record = request->record();
    const auto* descriptor = record.descriptor();
    std::ostringstream cmd;
    cmd << "insert into " << descriptor->name() << " ("
           "`when`, "
           "`payer`, "
           "`payee`, "
           "`amount`, "
           "`clasify`, "
           "`comments`, "
           "`create_time`, "
           "`update_time`"
           ") values (";
    cmd << "\"" << record.when() << "\", ";
    cmd << "\"" << record.payer() << "\", ";
    cmd << "\"" << record.payee() << "\", ";
    cmd << record.amount() << ", ";
    cmd << record.clasify() << ", ";
    cmd << "\"" << record.comments() << "\", ";

    char buf[50];
    struct tm tm;
    time_t now = time(nullptr);
    localtime_r(&now, &tm);
    strftime(buf, 50, "%F %T", &tm);
    cmd << "\"" << std::string(buf) << "\", ";
    cmd << "\"" << std::string(buf) << "\"";
    cmd << ")";

    bool suc = mysql_instance->Execute(cmd.str());
    financial_all_records(response);

    response->set_code(suc ? financial_response_rcode_ok : financial_response_rcode_dberr);
    return;
}

void JarvisServiceImpl::DeleteFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_request* request,
                       ::jarvis::financial_response* response,
                       ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    BLOCK_OPTIONS_REQUEST

    const auto& record = request->record();
    std::ostringstream cmd;
    cmd << "delete from financial_records where id = " << record.id();
    mysql_instance->Execute(cmd.str());
    financial_all_records(response);
    response->set_code(financial_response_rcode_ok);
}

void JarvisServiceImpl::UpdateFinancialRecord(::google::protobuf::RpcController* controller,
                    const ::jarvis::financial_request* request,
                    ::jarvis::financial_response* response,
                    ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    BLOCK_OPTIONS_REQUEST

    const auto& record = request->record();
    const auto* descriptor = record.descriptor();
    std::ostringstream cmd;
    cmd << "update " << descriptor->name() << " set ";
    cmd << "`when` = '" << record.when() << "', ";
    cmd << "`payer` = " << record.payer() << ", ";
    cmd << "`payee` = " << record.payee() << ", ";
    cmd << "`amount` = " << record.amount() << ", ";
    cmd << "`clasify` = " << record.clasify() << ", ";
    cmd << "`comments` = '" << record.comments() << "', ";

    char buf[50];
    struct tm tm;
    time_t now = time(nullptr);
    localtime_r(&now, &tm);
    strftime(buf, 50, "%F %T", &tm);
    cmd << "`update_time` = '" << std::string(buf) << "'";
    cmd << " where `id` = " << record.id();

    bool suc = mysql_instance->Execute(cmd.str());
    financial_all_records(response);

    response->set_code(suc ? financial_response_rcode_ok : financial_response_rcode_dberr);
}


} // namespace jarvis
