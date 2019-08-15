cmake_minimum_required(VERSION 3.10)

add_executable(unit-tests test_null.cpp)
add_library(unit-tests-lib test_null.cpp)
include_directories(../lib/catch)