cmake_minimum_required(VERSION 3.12)

project(logagent LANGUAGES "C")

# static library dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/output/lib)
# dynamic library dir
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/output/lib)
# default library dir
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/output/lib)
# exe file dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/output/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/agent)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/agent AGENT_SRC)

set(AGENT_NAME log_agent)
add_executable(${AGENT_NAME} ${AGENT_SRC})
target_link_libraries(${AGENT_NAME} asynclog)

set(TEST_NAME test)
add_executable(${TEST_NAME} ${AGENT_SRC})
target_link_libraries(${TEST_NAME} asynclog pthread)

set(install_prefix "/usr/bin")
install(TARGETS ${AGENT_NAME} RUNTIME DESTINATION ${install_prefix})
