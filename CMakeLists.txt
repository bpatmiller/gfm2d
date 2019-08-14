# project defn
cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME gfm)
project(${PROJECT_NAME})



# cpp version / settings
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
SET(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
SET(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} --coverage")

# cmake options
set(CMAKE_BUILD_TYPE COVERAGE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# source / lib directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
aux_source_directory(${SRC_DIR} SOURCES)
add_executable(${PROJECT_NAME} ${SOURCES})

# # coverage
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# include(CodeCoverage)
# APPEND_COVERAGE_COMPILER_FLAGS()
# set(COVERAGE_LCOV_EXCLUDES 'inc/*')

# PkgConfig
find_package(PkgConfig REQUIRED)

# GLM
SET(expected_glm_dir ${CMAKE_SOURCE_DIR}/lib/glm)
INCLUDE_DIRECTORIES(BEFORE SYSTEM ${expected_glm_dir})
message(STATUS "~Including bundled glm at ${expected_glm_dir}")
add_definitions(-DGLM_ENABLE_EXPERIMENTAL -DGLM_FORCE_SIZE_FUNC=1 -DGLM_FORCE_RADIANS=1)

# json and catch2
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
message(STATUS "~Including json/catch2 in inc/")

# Eigen
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(Eigen3 REQUIRED NO_MODULE)
include_directories("${EIGEN3_INCLUDE_DIR}")

message(STATUS "~Including eigen3 at ${EIGEN3_INCLUDE_DIR}")
