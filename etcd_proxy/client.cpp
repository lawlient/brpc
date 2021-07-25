#include "echo.pb.h"
#include "etcd_client.h"


#include <gflags/gflags.h>
#include <butil/logging.h>
#include <butil/time.h>
#include <brpc/channel.h>



DEFINE_string(service_name, "test-register", "default service name");

int main(int argc, char* argv[]) {
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);

    put(FLAGS_service_name);

    ServiceAddr addr;
    get(FLAGS_service_name, addr);

    return 0;
}

