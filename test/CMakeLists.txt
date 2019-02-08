cmake_minimum_required(VERSION 3.10)
project(mal-test CXX)

set(CMAKE_CXX_STANDARD 11)

include_directories(contrib)
include_directories(contrib/gtest)
include_directories(timer)
include_directories(utils)
include_directories(../library/src)

add_definitions(-DMAL_GNU -DMAL_FLOAT)

add_executable(mal-test
        contrib/gtest/gtest-all.cc
        contrib/gtest/gtest.h
        dac/test_mal_dac.cpp
        mal/mal_config.c
        timer/test_mal_timer.cpp
        timer/test_mal_timer.h
        utils/test_mal_circular_buffer.cpp
        utils/test_mal_circular_buffer.h
        utils/test_mal_e3prom.cpp
        utils/test_mal_pool.cpp
        main.cpp utils/test_mal_async_e3prom.cpp)
target_link_libraries(mal-test ${MAL_LIB_PATH})
