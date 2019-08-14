cmake_minimum_required(VERSION 3.10)

add_executable(unit-tests test_null.cpp)
# add_library(test_null test_null.cpp)
include_directories(${PROJECT_SOURCE_DIR}/inc)