/* Copyright 2008, 2010, Oracle and/or its affiliates.

 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * There are special exceptions to the terms and conditions of the GPL
 * as it is applied to this software. View the full text of the
 * exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
 * software distribution.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * */

#include "jarvis.h"
#include "redis_client.h"

#include <log/asynclog.h>
#include <config/config.h>

#include <butil/logging.h>
#include <brpc/server.h>

/* Standard C++ includes */
#include <iostream>

#include <gflags/gflags.h>



int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);

    auto* cfg = basis::text_config::TextConfig::GetInstance();
    if (!cfg->Init()) {
        LOG(ERROR) << "init server config fail";
        return false;
    }

    auto alog = basis::asynclog::AsyncLog::GetInstance();
    if (alog) alog->Init();

    // Generally you only need one Server.
    brpc::Server server;

    // Instance of your service.
    jarvis::JarvisServiceImpl jarvis;

    // Add the service into server. Notice the second parameter, because the
    // service is put on stack, we don't want server to delete it, otherwise
    // use brpc::SERVER_OWNS_SERVICE.
    if (server.AddService(&jarvis, brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service";
        return -1;
    }

    butil::EndPoint point;
    point = butil::EndPoint(butil::IP_ANY, cfg->cfg().port());

    // Start the server.
    brpc::ServerOptions options;
    options.idle_timeout_sec = cfg->cfg().idle_tmo_s() ? cfg->cfg().idle_tmo_s() : -1;
    if (server.Start(point, &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();

    return EXIT_SUCCESS;
}
