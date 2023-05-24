cmake_minimum_required(VERSION 3.23)
project(cpp_example)

set(CMAKE_CXX_STANDARD 14)

# 查找 pthread 库
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# demo executable
add_executable(cpp_example main.cpp)

# shared_ptr / unique_ptr / weak_ptr
add_subdirectory(shared_ptr)
# right reference
add_subdirectory(rvalue_reference)
# lambda
add_subdirectory(lambda)
# thread
add_subdirectory(thread)
# mutex
add_subdirectory(mutex)
# condition variable
add_subdirectory(condition_variable)
# atomic
add_subdirectory(atomic)
# future
add_subdirectory(future)
# other concurrent tools
add_subdirectory(concurrent_tools)
# variable parameter
add_subdirectory(variable_parameter)
# thread pool
add_subdirectory(thread_pool)
# exception
add_subdirectory(exception)
# network
add_subdirectory(network)
# valgrind
add_subdirectory(valgrind)










