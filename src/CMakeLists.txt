aux_source_directory(${CMAKE_CURRENT_LIST_DIR} src)
add_executable(${PROJECT_NAME} ${src})

target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}lib)
include_directories(${PROJECT_ROOT}/lib)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()