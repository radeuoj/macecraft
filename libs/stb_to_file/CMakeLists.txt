cmake_minimum_required(VERSION 3.20)
project(stb_to_file)

add_executable(stb_to_file main.cpp)

target_link_libraries(stb_to_file stb_image)