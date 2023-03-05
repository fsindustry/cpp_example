cmake_minimum_required(VERSION 3.23)
project(cpp_example)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

# shared_ptr / unique_ptr / weak_ptr
add_executable(cpp_example main.cpp)
add_executable(shared_ptr shared_ptr/shared_ptr_example.cpp)
add_executable(unique_ptr unique_ptr/unique_ptr_example.cpp)
add_executable(weak_ptr weak_ptr/weak_ptr_example.cpp)

# right reference
add_executable(rvalue_reference rvalue_reference/rvalue_reference.cpp)
add_executable(demo1_shallow_copy rvalue_reference/demo1_shallow_copy.cpp)
add_executable(demo1_deep_copy rvalue_reference/demo1_deep_copy.cpp)
add_executable(demo1_move_constructor rvalue_reference/demo1_move_constructor.cpp)
add_executable(demo2_move_semantics rvalue_reference/demo2_move_semantics.cpp)
add_executable(demo3_forward rvalue_reference/demo3_forward.cpp)
add_executable(demo4_emplace_back rvalue_reference/demo4_emplace_back.cpp)
add_executable(demo5_emplace_back rvalue_reference/demo5_emplace_back.cpp)

# lambda
add_executable(demo1_lambda lambda/demo1_lambda.cpp)

# thread
add_executable(demo1_thread_quickstart thread/demo1_thread_quickstart.cpp)
add_executable(demo2_thread_basic thread/demo2_thread_basic.cpp)

# mutex
add_executable(demo1_mutex mutex/demo1_mutex.cpp)
add_executable(demo2_recursive_mutex mutex/demo2_recursive_mutex.cpp)
add_executable(demo3_timed_mutex mutex/demo3_timed_mutex.cpp)
add_executable(demo4_lock_guard mutex/demo4_lock_guard.cpp)
add_executable(demo5_unique_lock mutex/demo5_unique_lock.cpp)





