cmake_minimum_required(VERSION 3.16)

project(Cmd-line-chat C)

set(CMAKE_C_VERSION 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(cl_talk cl_talk.c)
add_executable(cl_reply cl_reply.c)
add_executable(cl_request cl_request.c)
target_link_libraries(cl_talk pthread)
