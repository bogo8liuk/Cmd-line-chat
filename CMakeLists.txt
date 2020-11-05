cmake_minimum_required(VERSION 3.16)

project(Cmd-line-chat C)

set(CMAKE_C_VERSION 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(cl_utils STATIC ${CMAKE_CURRENT_SOURCE_DIR}/cl_utils.c)
add_executable(cl_reply cl_reply.c)
add_executable(cl_request cl_request.c)
target_link_libraries(cl_reply cl_utils)
target_link_libraries(cl_request cl_utils)
