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

cmake_minimum_required(VERSION 3.12)

option(LINK_SO "Whether examples are linked dynamically" OFF)

execute_process(
    COMMAND bash -c "find ${PROJECT_SOURCE_DIR}/../.. -type d -regex \".*output/include$\" | head -n1 | xargs dirname | tr -d '\n'"
    OUTPUT_VARIABLE OUTPUT_PATH
)

include(FindThreads)
include(FindProtobuf)

# Search for libthrift* by best effort. If it is not found and brpc is
# compiled with thrift protocol enabled, a link error would be reported.
find_library(THRIFT_LIB NAMES thrift)
if (NOT THRIFT_LIB)
    set(THRIFT_LIB "")
endif()
find_library(THRIFTNB_LIB NAMES thriftnb)
if (NOT THRIFTNB_LIB)
    set(THRIFTNB_LIB "")
endif()

find_path(BRPC_INCLUDE_PATH NAMES brpc/server.h)
if(LINK_SO)
    find_library(BRPC_LIB NAMES brpc)
else()
    find_library(BRPC_LIB NAMES libbrpc.a brpc)
endif()
if((NOT BRPC_INCLUDE_PATH) OR (NOT BRPC_LIB))
    message(FATAL_ERROR "Fail to find brpc")
endif()
include_directories(${BRPC_INCLUDE_PATH})

find_path(GFLAGS_INCLUDE_PATH gflags/gflags.h)
find_library(GFLAGS_LIBRARY NAMES gflags libgflags)
if((NOT GFLAGS_INCLUDE_PATH) OR (NOT GFLAGS_LIBRARY))
    message(FATAL_ERROR "Fail to find gflags")
endif()
include_directories(${GFLAGS_INCLUDE_PATH})

execute_process(
    COMMAND bash -c "grep \"namespace [_A-Za-z0-9]\\+ {\" ${GFLAGS_INCLUDE_PATH}/gflags/gflags_declare.h | head -1 | awk '{print $2}' | tr -d '\n'"
    OUTPUT_VARIABLE GFLAGS_NS
)
if(${GFLAGS_NS} STREQUAL "GFLAGS_NAMESPACE")
    execute_process(
        COMMAND bash -c "grep \"#define GFLAGS_NAMESPACE [_A-Za-z0-9]\\+\" ${GFLAGS_INCLUDE_PATH}/gflags/gflags_declare.h | head -1 | awk '{print $3}' | tr -d '\n'"
        OUTPUT_VARIABLE GFLAGS_NS
    )
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    include(CheckFunctionExists)
    CHECK_FUNCTION_EXISTS(clock_gettime HAVE_CLOCK_GETTIME)
    if(NOT HAVE_CLOCK_GETTIME)
        set(DEFINE_CLOCK_GETTIME "-DNO_CLOCK_GETTIME_IN_MAC")
    endif()
endif()

set(CMAKE_CPP_FLAGS "${DEFINE_CLOCK_GETTIME} -DGFLAGS_NS=${GFLAGS_NS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -DNDEBUG -O2 -D__const__=__unused__ -pipe -W -Wall -g -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

if(CMAKE_VERSION VERSION_LESS "3.1.3")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

find_path(LEVELDB_INCLUDE_PATH NAMES leveldb/db.h)
find_library(LEVELDB_LIB NAMES leveldb)
if ((NOT LEVELDB_INCLUDE_PATH) OR (NOT LEVELDB_LIB))
    message(FATAL_ERROR "Fail to find leveldb")
endif()
include_directories(${LEVELDB_INCLUDE_PATH})

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(OPENSSL_ROOT_DIR
        "/usr/local/opt/openssl"    # Homebrew installed OpenSSL
        )
endif()

find_package(OpenSSL)
include_directories(${OPENSSL_INCLUDE_DIR})

set(DYNAMIC_LIB
    ${CMAKE_THREAD_LIBS_INIT}
    ${GFLAGS_LIBRARY}
    ${PROTOBUF_LIBRARIES}
    ${LEVELDB_LIB}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
    ${THRIFT_LIB}
    ${THRIFTNB_LIB}
    dl
    )

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(DYNAMIC_LIB ${DYNAMIC_LIB}
        pthread
        "-framework CoreFoundation"
        "-framework CoreGraphics"
        "-framework CoreData"
        "-framework CoreText"
        "-framework Security"
        "-framework Foundation"
        "-Wl,-U,_MallocExtension_ReleaseFreeMemory"
        "-Wl,-U,_ProfilerStart"
        "-Wl,-U,_ProfilerStop")
endif()




## -----------  common part ---------------

set(COMMON ${CMAKE_CURRENT_LIST_DIR})
set(COMMON_CONFIG_PATH ${COMMON}/proto)

include_directories(${COMMON})
include_directories(${COMMON}/config)
include_directories(${COMMON}/mysql)
include_directories(${COMMON}/redis)
include_directories(${COMMON}/log)
include_directories(${COMMON}/thirdparth)


## nlohmann-json
find_package(nlohmann_json 3.2.0 REQUIRED)


function (auto_gen_client_code out_path proto_file)
    cmake_parse_arguments(common "no_default_client;gen_pb;" "" "proto_path_list" ${ARGN})

    file(MAKE_DIRECTORY ${out_path})
    message(STATUS ${out_path})

    foreach(p ${common_proto_path_list})
        set(proto_path_list ${proto_path_list} "--proto_path=${p}")
    endforeach()
    message(STATUS "proto_path_list: ${proto_path_list}")

    # if(asyn_cb_cntl)
    #     message(STATUS "async client callback with controller.")
    #     set(cxx_opt_list "--cxx_opt=asyn_cb_cntl")
    # endif()
    if(common_no_default_client)
        message(STATUS "not generate default client.")
        set(cxx_opt_list ${cxx_opt_list} "--cxx_opt=no_default_client")
    endif()
    message(STATUS "cxx_opt_list: ${cxx_opt_list}")

    message(STATUS ${proto_file})
    foreach(f ${proto_file})
        execute_process(COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --cxx_out=${out_path} ${proto_path_list} ${cxx_opt_list} ${f})
        if (common_gen_pb)
            execute_process(COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --cpp_out=${out_path} ${proto_path_list} --proto_path=${COMMON_CONFIG_PATH} ${f})
        endif()

    endforeach()
endfunction()



function(add_common_source SRCS LIBS)
    cmake_parse_arguments(common "ASYNCLOG;MYSQL;REDIS;TEXTCONFIG" "" "" "" ${ARGN})
    if (common_ASYNCLOG)
        aux_source_directory(${COMMON}/log/ _src)
        list(APPEND SRCS ${_src})
        list(APPEND LIBS ${COMMON}/thirdparty/asynclog/libasynclog.a)

        message(STATUS "enable async log")
    endif()
    if (common_MYSQL)
        find_library(_mysql mysqlcppconn)
        list(APPEND LIBS ${_mysql})
        aux_source_directory(${COMMON}/mysql/ _src)
        list(APPEND SRCS ${_src})

        message(STATUS "enable mysql conn")
    endif()
    if (common_REDIS)
        aux_source_directory(${COMMON}/redis/ _src)
        list(APPEND SRCS ${_src})

        message(STATUS "enable redis client")
    endif()
    if (common_TEXTCONFIG)
        aux_source_directory(${COMMON}/config/ _src)
        list(APPEND SRCS ${_src})

        message(STATUS "enable text config")
    endif()


    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${LIBS} ${${LIBS}} PARENT_SCOPE)
endfunction()



