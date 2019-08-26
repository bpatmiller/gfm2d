cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME "gfm")
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})

project(${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

### TODO consider changing coverage methodology?
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} --coverage")

if(DEFINED ENV{COVERAGE_OPTION})
  set(CMAKE_BUILD_TYPE COVERAGE)
  message(STATUS "OPT building with coverage options !!")
else()
  set(CMAKE_BUILD_TYPE RELEASE)
  message(STATUS "OPT building without coverage options ~~")
endif()
###

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_ROOT}/cmake/")

add_subdirectory(lib)
add_subdirectory(src)