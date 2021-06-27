#include <gflags/gflags.h>
#include <butil/logging.h>
#include <butil/time.h>
#include <brpc/channel.h>
#include "echo.pb.h"

#include "util/sys_helper.h"
#include "util/base64.h"


#include <nlohmann/json.hpp>




DEFINE_string(service_name, "test-register", "default service name");
DEFINE_string(service_port, "9527", "default service port");


DEFINE_string(etcd_addr, "172.26.15.113:2379", "default address of etcd");
DEFINE_int32(timeout_ms, 100, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 1, "Max retries(not including the first RPC)"); 
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");

int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);

    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;
    const std::string url = "http://" + FLAGS_etcd_addr;
    const std::string interface = "/v3/kv/range";
    
    brpc::ChannelOptions options;
    options.protocol = brpc::PROTOCOL_HTTP;  // or brpc::PROTOCOL_H2
    if (channel.Init(url.c_str() /*any url*/, &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return 1;
    }

    brpc::Controller cntl;
    cntl.http_request().uri() = url + interface;  // 设置为待访问的URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    nlohmann::json addr;
    
    addr["key"] = sys::base64_encode(FLAGS_service_name);
    char ip[16];
    if (0 == sys::get_local_ip("eth0", ip)) {
        addr["value"] = sys::base64_encode(std::string(ip) + ":" + FLAGS_service_port);
    }
    cntl.request_attachment().append(addr.dump());
    
    LOG(INFO) << "uri:" << cntl.http_request().uri() << " " << cntl.request_attachment().to_string();
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);
    if (cntl.Failed()) {
        LOG(ERROR) << "ErrorText:" << cntl.ErrorText() << cntl.remote_side();
        return 2;
    }

    auto buf = cntl.response_attachment();
    LOG(INFO) << buf.to_string();

    return 0;
}

