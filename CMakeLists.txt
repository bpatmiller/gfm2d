# project defn
cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME gfm)
project(${PROJECT_NAME})

# cmake options
set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# cpp version / settings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# source / lib directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
aux_source_directory(${SRC_DIR} SOURCES)
add_executable(${PROJECT_NAME} ${SOURCES})

# PkgConfig
find_package(PkgConfig REQUIRED)

# json
find_package(nlohmann_json REQUIRED)
target_link_libraries(gfm nlohmann_json::nlohmann_json)
message(STATUS "nlohmann-json included")

# Eigen
find_package(Eigen3 REQUIRED)
include_directories(BEFORE SYSTEM ${EIGEN3_INCLUDE_DIR})
message(STATUS "Eigen included at ${EIGEN3_INCLUDE_DIR}")
