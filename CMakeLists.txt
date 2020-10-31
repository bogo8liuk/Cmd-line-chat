cmake_minimum_required(VERSION 3.10)

project(Cmd-line-chat C)

set(CMAKE_C_VERSION 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

add_executable(clchat_response clchat_response.c)
add_executable(clchat_request clchat_request.c)
