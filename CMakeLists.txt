cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(my-sip)
include_directories(src/include)
aux_source_directory(src SIP_SRC_DIR)
add_subdirectory(src/sip)

enable_testing()
add_subdirectory(third-party/googletest)
include_directories(third-party/googletest/include)
add_executable(test-parser tests/sip-parser/test-basic.cpp)
target_link_libraries(test-parser gtest gtest_main SipParser)
add_test(NAME test-parser COMMAND test-parser)

add_executable(my-sip ${SIP_SRC_DIR} src/sdp/sdp.cpp src/socket/raw-socket.cpp src/rtp/rtp-socket.cpp)
target_link_libraries(my-sip SipParser)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
endif()

add_executable(parser-benchmark tests/parser-benchmark.cpp)
target_link_libraries(parser-benchmark SipParser)