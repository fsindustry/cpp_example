# condition variable
add_executable(demo1_sync_queue demo1_sync_queue.cpp)
target_link_libraries(demo1_sync_queue PRIVATE Threads::Threads)

add_executable(demo2_sync_queue demo2_sync_queue.cpp)
target_link_libraries(demo2_sync_queue PRIVATE Threads::Threads)
