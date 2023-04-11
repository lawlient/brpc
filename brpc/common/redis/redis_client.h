#pragma once

#include <brpc/redis.h>
#include <brpc/channel.h>


namespace redis {

struct RedisClientOption {
    std::string url;
};

class RedisClient {
public:
    explicit RedisClient(const RedisClientOption& opt);

    bool set();

private:
    RedisClient() = default;

private:
    RedisClientOption m_option;
    brpc::Channel m_channel;
};




    
} // namespace redis


