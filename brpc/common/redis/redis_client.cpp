#include "redis_client.h"

namespace redis {


RedisClient::RedisClient(const RedisClientOption& opt) : m_option(opt) {
    brpc::ChannelOptions options;
    options.protocol = brpc::PROTOCOL_REDIS;
    if (m_channel.Init(m_option.url.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to init channel to redis server, url:" << m_option.url;
    }
}

bool RedisClient::set() {
    brpc::RedisRequest set_request;
    brpc::RedisResponse response;
    brpc::Controller cntl;

    std::string key = "kkk";
    std::string val = "vvv";
    set_request.AddCommand("SET %s %s", key.c_str(), val.c_str());
    m_channel.CallMethod(NULL, &cntl, &set_request, &response, NULL/*done*/);
    if (cntl.Failed()) {
        LOG(ERROR) << "Fail to access redis-server";
        return false;
    }
    // 可以通过response.reply(i)访问某个reply
    if (response.reply(0).is_error()) {
        LOG(ERROR) << "Fail to set";
        return false;
    }
    // 可用多种方式打印reply
    LOG(INFO) << response.reply(0).c_str()  // OK
              << response.reply(0)          // OK
              << response;                  // OK

    return true;
}


} // namespace redis