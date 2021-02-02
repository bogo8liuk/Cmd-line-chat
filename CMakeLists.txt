cmake_minimum_required(VERSION 3.16)

project(Cmd-line-chat C)

set(CMAKE_C_VERSION 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

set(SYMBOLS cl_utils.h cl_talk.h)

include_directories(./)
add_library(LINK cl_utils.c cl_talk.c)
add_executable(cl_reply cl_reply.c)
add_executable(cl_request cl_request.c)
target_link_libraries(cl_reply PRIVATE LINK)
target_link_libraries(cl_request PRIVATE LINK)
