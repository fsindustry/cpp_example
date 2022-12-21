cmake_minimum_required(VERSION 3.23)
project(cpp_example)

set(CMAKE_CXX_STANDARD 14)

add_executable(cpp_example main.cpp)
add_executable(shared_ptr shared_ptr/shared_ptr_example.cpp)
add_executable(unique_ptr unique_ptr/unique_ptr_example.cpp)