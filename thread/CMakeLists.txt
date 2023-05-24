# thread
add_executable(demo1_thread_quickstart demo1_thread_quickstart.cpp)
target_link_libraries(demo1_thread_quickstart PRIVATE Threads::Threads)
add_executable(demo2_thread_basic demo2_thread_basic.cpp)
target_link_libraries(demo2_thread_basic PRIVATE Threads::Threads)

