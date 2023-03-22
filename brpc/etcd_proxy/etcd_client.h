#pragma once

#include<string>
#include<vector>

using ServiceName = std::string;
struct ServiceAddr {
    std::vector<std::string> iplist;
};

int put(const ServiceName&);
int get(const ServiceName&, ServiceAddr&);