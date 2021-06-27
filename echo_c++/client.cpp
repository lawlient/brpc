#include <gflags/gflags.h>
#include <butil/logging.h>
#include <butil/time.h>
#include <brpc/channel.h>
#include "echo.pb.h"



DEFINE_string(attachment, "", "Carry this along with requests");
DEFINE_string(protocol, "baidu_std", "Protocol type. Defined in src/brpc/options.proto");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
DEFINE_int32(timeout_ms, 100, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)"); 
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");

int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);

    // A Channel represents a communication line to a Server. Notice that 
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;
    static const char* ip = "172.30.146.135";
    const uint32_t port = 2379;
    const std::string url = "http://" + std::string(ip) + ":" + std::to_string(port);
    const std::string interface = "/v3/kv/range";
    
    brpc::ChannelOptions options;
    options.protocol = brpc::PROTOCOL_HTTP;  // or brpc::PROTOCOL_H2
    if (channel.Init(url.c_str() /*any url*/, &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return;
    }

    brpc::Controller cntl;
    cntl.http_request().uri() = url + interface;  // 设置为待访问的URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl.request_attachment().append("{\"key\":\"Zm9v\"}");
    
    LOG(INFO) << "uri:" << cntl.http_request().uri() << " " << cntl.request_attachment().to_string();
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);
    if (cntl.Failed()) {
        LOG(ERROR) << "ErrorText:" << cntl.ErrorText() << cntl.remote_side();
        return;
    }

    auto buf = cntl.response_attachment();
    LOG(INFO) << buf.to_string();
    return;

    LOG(INFO) << "EchoClient is going to quit";
    return 0;
}

