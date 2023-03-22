aux_source_directory(. LIB)
message(STATUS "lib source : ${LIB}")
add_library(asynclog STATIC ${LIB})
