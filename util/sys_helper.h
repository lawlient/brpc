#pragma once

namespace sys {

extern int get_ip_by_domain(const char *domain, char *ip); // 根据域名获取ip
extern int get_local_mac(const char *eth_inf, char *mac); // 获取本机mac
extern int get_local_ip(const char *eth_inf, char *ip); // 获取本机ip
    
} // namespace sys