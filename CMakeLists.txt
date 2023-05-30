cmake_minimum_required(VERSION 3.0.0)
project(pong VERSION 0.1.0)

add_executable(pong src/main.c)

target_link_libraries(pong SDL2)