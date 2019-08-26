include_directories(${CMAKE_CURRENT_LIST_DIR})

set(lib_src ${CMAKE_CURRENT_LIST_DIR})
aux_source_directory(${CMAKE_CURRENT_LIST_DIR} lib_src)
add_library(${PROJECT_NAME}lib STATIC ${lib_src})

find_package(GLM 0.9.5 REQUIRED)
include_directories(SYSTEM ${GLM_INCLUDE_DIRS})

find_package(Eigen3 3.3.0 REQUIRED NO_MODULE)
include_directories(${EIGEN3_INCLUDE_DIR})

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(${PROJECT_NAME}lib PRIVATE nlohmann_json::nlohmann_json)