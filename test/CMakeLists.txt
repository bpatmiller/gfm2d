cmake_minimum_required(VERSION 3.10)

add_executable(unit-tests test_null.cpp)
include_directories(../lib/catch)
include_directories(../lib/glm)
include_directories(../src)
