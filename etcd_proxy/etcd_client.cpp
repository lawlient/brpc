#include "etcd_client.h"
#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/channel.h>

#include "util/sys_helper.h"
#include "util/base64.h"


#include <nlohmann/json.hpp>

DEFINE_int32(service_port, 9527, "default service port");

DEFINE_string(etcd_addr, "172.22.238.81:2379", "default address of etcd");
DEFINE_int32(timeout_ms, 100, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 1, "Max retries(not including the first RPC)"); 
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");

static const std::string kInterfacePrefix = "/v3/kv/";

int get(const ServiceName& sname, ServiceAddr& addr) {
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = brpc::PROTOCOL_HTTP;  // or brpc::PROTOCOL_H2
    const std::string kEtcdUrl = "http://" + FLAGS_etcd_addr;
    if (channel.Init(kEtcdUrl.c_str() /*any url*/, &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return 1;
    }

    brpc::Controller cntl;
    cntl.http_request().uri() = kEtcdUrl + kInterfacePrefix + "range";  // 设置为待访问的URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    nlohmann::json jreq;
    
    jreq["key"] = sys::base64_encode(sname);
    cntl.request_attachment().append(jreq.dump());
    
    LOG(INFO) << "uri:" << cntl.http_request().uri() << " " << cntl.request_attachment().to_string();
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);
    if (cntl.Failed()) {
        LOG(ERROR) << "ErrorText:" << cntl.ErrorText() << " remote:" << cntl.remote_side();
        return 2;
    }

    auto buf = cntl.response_attachment();
    nlohmann::json jres = nlohmann::json::parse(buf.to_string(), nullptr, false);
    auto hit = jres.find("header");
    if (hit != jres.end()) {
        auto cit = hit->find("cluster_id");
        if (cit != hit->end()) {
            std::cout << "cluster_id:" << *cit;
        }
    }

    return 0;
}

int put(const ServiceName& name) {
    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;
    
    brpc::ChannelOptions options;
    options.protocol = brpc::PROTOCOL_HTTP;  // or brpc::PROTOCOL_H2
    const std::string kEtcdUrl = "http://" + FLAGS_etcd_addr;
    if (channel.Init(kEtcdUrl.c_str() /*any url*/, &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return 1;
    }

    brpc::Controller cntl;
    cntl.http_request().uri() = kEtcdUrl + kInterfacePrefix + "put";  // 设置为待访问的URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    nlohmann::json addr;
    
    addr["key"] = sys::base64_encode(name);
    char ip[16];
    if (0 == sys::get_local_ip("eth0", ip)) {
        addr["value"] = sys::base64_encode(std::string(ip) + ":" + std::to_string(FLAGS_service_port));
    }
    addr["lease"] = 3;
    cntl.request_attachment().append(addr.dump());
    
    LOG(INFO) << "uri:" << cntl.http_request().uri() << " " << cntl.request_attachment().to_string();
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);
    if (cntl.Failed()) {
        LOG(ERROR) << "ErrorText:" << cntl.ErrorText() << " remote:" << cntl.remote_side();
        return 2;
    }

    auto buf = cntl.response_attachment();
    LOG(INFO) << buf.to_string() << " success";

    return 0;
}

