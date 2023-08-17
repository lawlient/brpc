#include "jarvis.h"
#include "util.h"
#include "redis_client.h"

static const char* TOKEN = "token";

DEFINE_int32(login_cache_ttl, 300, "default seconds of login token caching");

namespace jarvis {


void JarvisServiceImpl::update_token_ttl() {
    auto* cfg = basis::text_config::TextConfig::GetInstance();
    const auto& redis_config = cfg->cfg().redis();
    redis::RedisClientOption redis_option;
    redis_option.url = redis_config.url();
    redis::RedisClient rclient(redis_option);
    rclient.expire(TOKEN, FLAGS_login_cache_ttl);
}

bool JarvisServiceImpl::check_login() {
    auto* cfg = basis::text_config::TextConfig::GetInstance();
    const auto& redis_config = cfg->cfg().redis();
    redis::RedisClientOption redis_option;
    redis_option.url = redis_config.url();
    redis::RedisClient rclient(redis_option);
    std::string token;
    rclient.get(TOKEN, &token);
    return 64 == token.size();
}


static bool get_login_user(mysql::MysqlInstance* db, const std::string& name, jarvis::jusers* usr) {
    mysql::SelectGenerator g(*usr);
    g.where("name = '" + name + "'");
    std::unique_ptr<mysql::Result> res(db->Execute(g.sql(), usr));
    bool success = false;
    for (const auto* msg : res->records) {
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

static bool check_login_info(mysql::MysqlInstance* db, const std::string& name, const std::string& secret) {
    jarvis::jusers usr;
    if (!get_login_user(db, name, &usr)) {
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
    if (!check_login_info(_db.get(), username, secret)) {
        nlohmann::json res;
        res["status"]         = 1;
        res["msg"]            = "password check failed";
        res["data"]["Hint"]   = "please contact with your admin!";
        cntl->response_attachment().append(res.dump());
        LOG(INFO) << "username: " << username << "secret: " << secret << " check failed. "
                  << cntl->response_attachment();
        return;
    }
    const auto& token = jutil::token_generator();
    auto* cfg = basis::text_config::TextConfig::GetInstance();
    const auto& redis_config = cfg->cfg().redis();
    redis::RedisClientOption redis_option;
    redis_option.url = redis_config.url();
    redis::RedisClient rclient(redis_option);
    rclient.set(TOKEN, token, FLAGS_login_cache_ttl);

    nlohmann::json res;
    res["status"]         = 0;
    res["msg"]            = "success";
    cntl->response_attachment().append(res.dump());
    LOG(INFO) << cntl->response_attachment();
}


} // namespace jarvis