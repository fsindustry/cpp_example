# other concurrent tools
add_executable(demo1_call_once demo1_call_once.cpp)
target_link_libraries(demo1_call_once PRIVATE Threads::Threads)

add_executable(demo2_function demo2_function.cpp)

add_executable(demo3_bind demo3_bind.cpp)
