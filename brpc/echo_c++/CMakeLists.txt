# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

cmake_minimum_required(VERSION 3.12.10)
project(echo_c++ C CXX)

set(COMMON ${CMAKE_CURRENT_LIST_DIR}/../common)
include(${COMMON}/common.cmake)

file(GLOB pbfiles
    "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto"
)
protobuf_generate_cpp(PB_SRCS PB_HDRS ${pbfiles})

#include PROTO_HEADER
include_directories(${CMAKE_CURRENT_BINARY_DIR})


set(CMAKE_PREFIX_PATH ${OUTPUT_PATH})
set(install_prefix "/data/services/echo/bin")
message(STATUS "Install Prefix: " ${install_prefix})


add_executable(echo_client ${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp ${PB_SRCS} ${PB_HDRS})
add_executable(echo_server ${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp ${PB_SRCS} ${PB_HDRS})

target_link_libraries(echo_client ${BRPC_LIB} ${DYNAMIC_LIB})
target_link_libraries(echo_server ${BRPC_LIB} ${DYNAMIC_LIB})

install(TARGETS echo_server RUNTIME DESTINATION ${install_prefix}/)