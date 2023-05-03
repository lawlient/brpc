#pragma once

#include <brpc/redis.h>
#include <brpc/channel.h>

#include <vector>


namespace redis {

struct RedisClientOption {
    std::string url;
};

class RedisClient {
public:
    explicit RedisClient(const RedisClientOption& opt);

    bool Execute(const std::string& cmd);
    
    bool set(const std::string& key, const std::string& val, int32_t ttl = 86400 * 30);
    bool get(const std::string& key, std::string* val);
    bool expire(const std::string& key, int32_t ttl);

private:
    RedisClient() = default;

private:
    RedisClientOption m_option;
    brpc::Channel m_channel;
};




    
} // namespace redis


