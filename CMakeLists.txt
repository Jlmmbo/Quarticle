cmake_minimum_required(VERSION 3.16)
project(Quarticle VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)

add_executable(Quarticle src/main.cpp src/glad.c)

target_include_directories(Quarticle PRIVATE include)

target_link_libraries(Quarticle PRIVATE glfw)
