cmake_minimum_required(VERSION 3.10)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
aux_source_directory(${SRC_DIR} SOURCES)
add_library(mainlib ${SOURCES})

add_executable(unit-tests test_calculus.cpp test_main.cpp)
include_directories(../lib/catch)
include_directories(../lib/glm)
include_directories(../src)
target_link_libraries(unit-tests mainlib)
