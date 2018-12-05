cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

project(allocator)
add_executable(${PROJECT_NAME} "main.cpp" "storage_manager.cpp")
