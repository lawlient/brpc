cmake_minimum_required(VERSION 3.12)


set(COMMON ${CMAKE_CURRENT_LIST_DIR})

set(COMMON_CONFIG_PATH ${COMMON}/proto)


include_directories(${COMMON})
include_directories(${COMMON}/config)

aux_source_directory(${COMMON}/config COMMON_SRC)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/client)
file(GLOB COMMON_PROTO "${CMAKE_CURRENT_LIST_DIR}/proto/*.proto")




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
