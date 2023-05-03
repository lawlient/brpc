#include "redis_client.h"

namespace redis {


RedisClient::RedisClient(const RedisClientOption& opt) : m_option(opt) {
    brpc::ChannelOptions options;
    options.protocol = brpc::PROTOCOL_REDIS;
    if (m_channel.Init(m_option.url.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to init channel to redis server, url:" << m_option.url;
    }
    LOG(INFO) << "Init channel to redis server success. url:" << m_option.url;
}

bool RedisClient::Execute(const std::string& cmd) {
    brpc::RedisRequest request;
    brpc::RedisResponse response;
    brpc::Controller cntl;

    request.AddCommand(cmd);
    m_channel.CallMethod(NULL, &cntl, &request, &response, NULL/*done*/);
    if (cntl.Failed()) {
        LOG(ERROR) << "Fail to access redis server";
        return false;
    }

    // 可以通过response.reply(i)访问某个reply
    if (response.reply(0).is_error()) {
        LOG(ERROR) << response.reply(0).error_message();
        return false;
    }
    // 可用多种方式打印reply
    LOG(INFO) << response;

    return true;
}

bool RedisClient::set(const std::string& key, const std::string& val, int32_t ttl) {
    brpc::RedisRequest request;
    brpc::RedisResponse response;
    brpc::Controller cntl;

    request.AddCommand("SET %s %s", key.c_str(), val.c_str());
    request.AddCommand("EXPIRE %s %d", key.c_str(), ttl);
    m_channel.CallMethod(NULL, &cntl, &request, &response, NULL/*done*/);
    if (cntl.Failed()) {
        LOG(ERROR) << "Fail to access redis server";
        return false;
    }

    // 可以通过response.reply(i)访问某个reply
    if (response.reply(0).is_error()) {
        LOG(ERROR) << response.reply(0).error_message();
        return false;
    }
    return true;
}

bool RedisClient::get(const std::string& key, std::string* val) {
    brpc::RedisRequest request;
    brpc::RedisResponse response;
    brpc::Controller cntl;

    request.AddCommand("GET %s", key.c_str());
    m_channel.CallMethod(NULL, &cntl, &request, &response, NULL/*done*/);
    if (cntl.Failed()) {
        LOG(ERROR) << "Fail to access redis server";
        return false;
    }

    // 可以通过response.reply(i)访问某个reply
    if (response.reply(0).is_error()) {
        LOG(ERROR) << response.reply(0).error_message();
        return false;
    }
    val->assign(response.reply(0).c_str());
    return true;
}

bool RedisClient::expire(const std::string& key, int32_t ttl) {
    brpc::RedisRequest request;
    brpc::RedisResponse response;
    brpc::Controller cntl;

    request.AddCommand("EXPIRE %s %d", key.c_str(), ttl);
    m_channel.CallMethod(NULL, &cntl, &request, &response, NULL/*done*/);
    if (cntl.Failed()) {
        LOG(ERROR) << "Fail to access redis server";
        return false;
    }

    // 可以通过response.reply(i)访问某个reply
    if (response.reply(0).is_error()) {
        LOG(ERROR) << response.reply(0).error_message();
        return false;
    }
    return true;
}

} // namespace redis