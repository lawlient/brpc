cmake_minimum_required(VERSION 3.22)

project(jarvis CXX)
set(exe "jarvis")

set(COMMON ${CMAKE_CURRENT_LIST_DIR}/../common)
include(${COMMON}/common.cmake)


set(CMAKE_PREFIX_PATH ${OUTPUT_PATH})
set(install_prefix "/data/services/jarvis/")
message(STATUS "Install Prefix: " ${install_prefix})

file(GLOB pbfiles
    "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto"
    "${COMMON}/proto/*.proto"
)
protobuf_generate_cpp(PB_SRCS PB_HDRS ${pbfiles})


#include PROTO_HEADER
include_directories(${CMAKE_CURRENT_BINARY_DIR})


# file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/client)
# file(GLOB COMMON_PROTO "${CMAKE_CURRENT_LIST_DIR}/proto/*.proto")

add_common_source(COMMON_SRCS COMMON_LIBS ASYNCLOG MYSQL TEXTCONFIG)
add_library(commonlibs ${PB_SRCS} ${PB_HDRS} ${COMMON_SRCS})
target_link_libraries(commonlibs ${COMMON_LIBS} nlohmann_json::nlohmann_json)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC)


add_executable(${exe} main.cpp ${SRC})
target_link_libraries(${exe} commonlibs ${BRPC_LIB} ${DYNAMIC_LIB})

install(TARGETS ${exe} RUNTIME DESTINATION ${install_prefix}/bin)
install(FILES ${CMAKE_CURRENT_LIST_DIR}/conf/default.conf DESTINATION ${install_prefix}/conf)
