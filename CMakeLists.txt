cmake_minimum_required(VERSION 3.16)

project(Cmd-line-chat C)

set(CMAKE_C_VERSION 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

include_directories(./)
add_library(LINK cl_utils.c)
add_executable(cl_reply cl_reply.c)
add_executable(cl_request cl_request.c)
target_link_libraries(cl_reply LINK)
target_link_libraries(cl_request LINK)

target_link_libraries(cl_reply pthread)
target_link_libraries(cl_request pthread)
