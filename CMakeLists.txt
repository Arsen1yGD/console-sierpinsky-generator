cmake_minimum_required(VERSION 4.0)

project(sierpinsky)

set (CMAKE_CXX_STANDARD 23)

add_executable(sierpinsky main.cpp)

install(TARGETS sierpinsky RUNTIME DESTINATION bin)
