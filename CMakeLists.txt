cmake_minimum_required(VERSION 3.0.0)

project(judger)

set(CMAKE_CXX_COMPILER "g++")
set (CMAKE_CXX_FLAGS  "-g -fexec-charset=GBK")
add_compile_options("-D__DEBUG")

include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(./src SrcFiles)
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)

find_library(SECCOMPLIB seccomp)

add_executable(judger ${SrcFiles})

target_link_libraries(judger ${SECCOMPLIB})