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

# condition variable
add_executable(demo1_sync_queue condition_variable/demo1_sync_queue.cpp)
add_executable(demo2_sync_queue condition_variable/demo2_sync_queue.cpp)

# atomic
add_executable(demo1_atomic_quickstart atomic/demo1_atomic_quickstart.cpp)

# future
add_executable(demo1_future future/demo1_future.cpp)
add_executable(demo2_package_task future/demo2_package_task.cpp)
add_executable(demo3_promise future/demo3_promise.cpp)

# other concurrent tools
add_executable(demo1_call_once concurrent_tools/demo1_call_once.cpp)
add_executable(demo2_function concurrent_tools/demo2_function.cpp)
add_executable(demo3_bind concurrent_tools/demo3_bind.cpp)

# variable parameter
add_executable(demo1_var_param variable_parameter/demo1_var_param.cpp)
add_executable(demo2_var_param_by_recursive variable_parameter/demo2_var_param_by_recursive.cpp)
add_executable(demo3_var_param_by_comma variable_parameter/demo3_var_param_by_comma.cpp)
add_executable(demo4_var_param_forward variable_parameter/demo4_var_param_forward.cpp)

# thread pool
add_executable(demo_threadpool thread_pool/test_threadpool.cpp thread_pool/demo_threadpool.cpp)

# exception
add_executable(demo1_exception_basic exception/demo1_exception_basic.cpp)
add_executable(demo2_custom_exception exception/demo2_custom_exception.cpp)
add_executable(demo3_exception_ptr exception/demo3_exception_ptr.cpp)
add_executable(demo4_nested_exception exception/demo4_nested_exception.cpp)










