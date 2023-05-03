#include "jarvis.h"
#include "mysql.h"
#include "redis_client.h"

#include <config/config.h>

#include <nlohmann/json.hpp>

#include <openssl/hmac.h>
#include <openssl/evp.h>

#include <google/protobuf/util/json_util.h>


using namespace google::protobuf;

DEFINE_int32(login_cache_ttl, 300, "default seconds of login token caching");

namespace jarvis {

static const int32_t kOwnerUid = 0; /* It is me */

static inline bool is_record_with_me(const jarvis::financial_records& r) {
    return (r.payer() == kOwnerUid || r.payee() == kOwnerUid);
}

static void update_token_ttl() {
    auto* cfg = basis::text_config::TextConfig::GetInstance();
    const auto& redis_config = cfg->cfg().redis();
    redis::RedisClientOption redis_option;
    redis_option.url = redis_config.url();
    redis::RedisClient rclient(redis_option);
    rclient.expire("token", FLAGS_login_cache_ttl);
}

static std::string token_generator();

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

static bool check_login() {
    auto* cfg = basis::text_config::TextConfig::GetInstance();
    const auto& redis_config = cfg->cfg().redis();
    redis::RedisClientOption redis_option;
    redis_option.url = redis_config.url();
    redis::RedisClient rclient(redis_option);
    std::string token;
    rclient.get("token", &token);
    return 64 == token.size();
}

static inline std::string nowstring() {
    char buf[50];
    struct tm tm;
    time_t now = time(nullptr);
    localtime_r(&now, &tm);
    strftime(buf, 50, "%F %T", &tm);
    return  std::string(buf);
}

/* 用户数相对有限， 全量查询 */
void financial_all_users(::jarvis::financial_users_response* response) {
    std::unique_ptr<sql::ResultSet> res;
    jarvis::financial_users users;
    res.reset(mysql_instance->SelectAll(users, "", "`uid` asc", "", "1000"));
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


void get_financial_record_size(int32_t* total) {
    *total = 0;
    std::unique_ptr<sql::ResultSet> res;
    jarvis::financial_records financial;
    std::ostringstream cmd;
    cmd << "select count(*) from "  << financial.descriptor()->name() << " where status = 0";
    res.reset(mysql_instance->ExecuteQuery(cmd.str()));
    if (!res) return;

    try {
        while (res->next()) {
            *total = res->getInt(1);
        }
    } catch(std::exception& e) {
        LOG(ERROR) << "what: " << e.what();
    }
}

/* 记录数不断追加，分页展示, 按id倒序，即最新数据在前 */
void financial_all_records(::jarvis::financial_response* response, int page = 0, int pagesize = 10) {
    std::unique_ptr<sql::ResultSet> res;
    jarvis::financial_records financial;
    std::string limit = std::to_string(page * pagesize) + ", " + std::to_string(pagesize);
    res.reset(mysql_instance->SelectAll(financial, "status = 0", "`id` desc", "", limit));
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

    int32_t total = 0;
    get_financial_record_size(&total);
    response->set_total(total);
}

bool get_user_balance(int32_t uid, jarvis::financial_asset* asset) {
    std::unique_ptr<sql::ResultSet> res;
    std::string where = "uid = " + std::to_string(uid);
    res.reset(mysql_instance->SelectAll(*asset, where, "`timestamp` desc", "", "1"));
    if (!res) return false;

    std::vector<google::protobuf::Message*> msgs;
    mysql_instance->Parse(res.get(), asset, msgs);

    bool success = false;
    for (const auto* msg : msgs) {
        const auto* row = dynamic_cast<const jarvis::financial_asset*>(msg);
        if (row == nullptr) {
            LOG(ERROR) << "cast financial asset fail";
            continue;
        }
        asset->CopyFrom(*row);
        success = true;
        LOG(DEBUG) << asset->ShortDebugString();
    }
    return success;
}

bool get_login_user(const std::string& name, jarvis::jusers* usr) {
    std::unique_ptr<sql::ResultSet> res;
    std::string where = "`name` = '" + name + "'";
    res.reset(mysql_instance->SelectAll(*usr, where, "", "", "1"));
    if (!res) return false;

    std::vector<google::protobuf::Message*> msgs;
    mysql_instance->Parse(res.get(), usr, msgs);

    bool success = false;
    for (const auto* msg : msgs) {
        const auto* row = dynamic_cast<const jarvis::jusers*>(msg);
        if (row == nullptr) {
            LOG(ERROR) << "cast login user fail";
            continue;
        }
        usr->CopyFrom(*row);
        success = true;
        LOG(DEBUG) << usr->ShortDebugString();
    }
    return success;   
}

bool update_user_balance(const jarvis::financial_asset& casset, 
                         const jarvis::financial_records& record) {
    if (record.payer() == record.payee()) return true; // ignore self to self

    double asset = casset.amount();
    if (record.payer() == casset.uid()) {
        asset -= record.amount();
    } else if (record.payee() == casset.uid()) {
        asset += record.amount();
    }

    const auto* descriptor  = casset.descriptor();
    const std::string nows  = nowstring();
    std::ostringstream cmd;
    cmd << "insert into " << descriptor->name() << " ("
           "`timestamp`, "
           "`uid`, "
           "`amount`, "
           "`recordid`, "
           "`create_time`, "
           "`update_time`"
           ") values (";
    cmd << "\"" << record.create_time() << "\", ";
    cmd << casset.uid() << ", ";
    cmd << asset << ", ";
    cmd << record.id() << ", ";
    cmd << "\"" << nows << "\", ";
    cmd << "\"" << nows << "\"";
    cmd << ")";

    bool suc = mysql_instance->Execute(cmd.str());
    return suc;
}




// ---------------------------   对外接口 ---------------------------------------

void JarvisServiceImpl::TestQuery(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::HttpResponse* response,
                       ::google::protobuf::Closure* done)  {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);

    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    LOG(INFO) << cntl->request_attachment();

    ::jarvis::financial_response tmp;
    financial_all_records(&tmp, 0, 10000);

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

static bool check_login_info(const std::string& name, const std::string& secret) {
    jarvis::jusers usr;
    if (!get_login_user(name, &usr)) {
        return false;
    }
    if (secret != usr.secret()) {
        LOG(INFO) << "secret check fail";
        return false;
    }
    return true;
}

void JarvisServiceImpl::Login(::google::protobuf::RpcController* controller,
                              const ::jarvis::HttpRequest* request,
                              ::jarvis::HttpResponse* response,
                              ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);

    const auto payload = cntl->request_attachment().to_string();
    nlohmann::json jpayload = nlohmann::json::parse(payload.c_str(), payload.c_str() + payload.size());
    
    const auto username = jpayload["username"].get<std::string>();
    const auto secret   = jpayload["secret"].get<std::string>();
    if (username.empty() || secret.empty()) {
        LOG(ERROR) << "username: " << username << "secret: " << secret;
        return;
    }
    if (!check_login_info(username, secret)) {
        nlohmann::json res;
        res["status"]         = 1;
        res["msg"]            = "password check failed";
        res["data"]["Hint"]   = "please contact with your admin!";
        cntl->response_attachment().append(res.dump());
        LOG(INFO) << "username: " << username << "secret: " << secret << " check failed. "
                  << cntl->response_attachment();
        return;
    }
    const auto& token = token_generator();
    auto* cfg = basis::text_config::TextConfig::GetInstance();
    const auto& redis_config = cfg->cfg().redis();
    redis::RedisClientOption redis_option;
    redis_option.url = redis_config.url();
    redis::RedisClient rclient(redis_option);
    rclient.set("token", token, FLAGS_login_cache_ttl);

    nlohmann::json res;
    res["status"]         = 0;
    res["msg"]            = "success";
    cntl->response_attachment().append(res.dump());
    LOG(INFO) << cntl->response_attachment();
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
    financial_all_users(&tmp);

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
    const std::string nows  = nowstring();

    std::ostringstream cmd;
    cmd << "update " << descriptor->name() << " set ";
    cmd << "`name` = " << user.name();
    cmd << "`update_time` = " << nows;
    cmd << " where `uid` = " << user.uid();

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
    mysql_instance->Execute(cmd.str());

    financial_all_users(response);
    response->set_code(financial_users_response_rcode_ok);
    return;
}

void JarvisServiceImpl::GetFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::HttpResponse* response,
                       ::google::protobuf::Closure* done)  {
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


    int page = 0, pagesize = 10;
    const auto& uri = cntl->http_request().uri();
    auto pagestr     = uri.GetQuery("page"); /* start from 1 */
    auto pagesizestr = uri.GetQuery("pagesize");
    if (pagestr && pagesizestr) {
        page = atoi(pagestr->c_str()) - 1;
        pagesize = atoi(pagesizestr->c_str());
    }

    ::jarvis::financial_response tmp;
    financial_all_records(&tmp, page, pagesize);

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

void JarvisServiceImpl::AppendFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_request* request,
                       ::jarvis::financial_response* response,
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


    const auto& record = request->record();
    const auto* descriptor = record.descriptor();
    const auto& nows       = nowstring();
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
    cmd << "\"" << nows << "\", ";
    cmd << "\"" << nows << "\"";
    cmd << ")";

    bool suc = mysql_instance->Execute(cmd.str());
    financial_all_records(response);

    if (suc && is_record_with_me(record)) {
        jarvis::financial_asset asset;
        if (get_user_balance(kOwnerUid, &asset)) {
            bool suc = update_user_balance(asset, response->records(0));
            if (!suc) LOG(ERROR) << "update my balance fail, record id:" << response->records(0).id();
        }
    }

    response->set_code(suc ? financial_response_rcode_ok : financial_response_rcode_dberr);
    return;
}

void JarvisServiceImpl::DeleteFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_request* request,
                       ::jarvis::financial_response* response,
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

    const auto& record = request->record();
    std::ostringstream cmd;
    cmd << "update from financial_records set status = 1 where id = " << record.id();
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
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);

    if (!check_login()) {
        nlohmann::json res;
        res["status"] = 1;
        res["msg"]    = "Please login";
        cntl->response_attachment().append(res.dump());
        return;
    }
    update_token_ttl();

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
    cmd << "`update_time` = '" << nowstring() << "'";
    cmd << " where `id` = " << record.id();

    bool suc = mysql_instance->Execute(cmd.str());
    financial_all_records(response);

    response->set_code(suc ? financial_response_rcode_ok : financial_response_rcode_dberr);
}

void JarvisServiceImpl::GetFinancialAsset(::google::protobuf::RpcController* controller,
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

    ::jarvis::financial_asset asset;
    get_user_balance(kOwnerUid, &asset);

    std::string data;
    google::protobuf::util::JsonPrintOptions joption;
    joption.always_print_primitive_fields = true;
    joption.preserve_proto_field_names = true;
    google::protobuf::util::MessageToJsonString(asset, &data, joption);
    std::ostringstream oss;
    oss << "{\"status\":0, \"msg\":\"Success\", \"data\":[" << data << "]}";
    cntl->response_attachment().append(oss.str());
    LOG(INFO) << cntl->response_attachment();
}


static std::string hmac_sha256(const unsigned char* data, size_t datalen) {
    static const char* KEY = "huba";
    unsigned char md[33];
    unsigned int mdlen = 0;

    HMAC(EVP_sha256(), KEY, strlen(KEY), data, datalen, md, &mdlen);

    char buf[65];
    for (unsigned int i = 0; i < mdlen; i++)
        snprintf(buf + 2*i, 65 - 2*i, "%.2x", md[i]);
    return std::string(buf);
}

std::string token_generator() { 
    const auto& msg = nowstring();
    return hmac_sha256((unsigned char*)msg.c_str(), msg.size());
}

} // namespace jarvis

