cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(sip-parser)
include_directories(src/include)
aux_source_directory(src SIP_SRC_DIR)

find_package(PkgConfig)
pkg_search_module(LUA53 REQUIRED lua53)

enable_testing()
add_subdirectory(third-party/googletest)
include_directories(third-party/googletest/include)
add_executable(test-parser tests/sip-parser/test-basic.cpp)
target_link_libraries(test-parser gtest gtest_main)
add_test(NAME test-parser COMMAND test-parser)

add_executable(sip-parser ${SIP_SRC_DIR})
target_link_libraries(sip-parser ${LUA53_LIBRARIES})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

add_executable(parser-benchmark tests/parser-benchmark.cpp)