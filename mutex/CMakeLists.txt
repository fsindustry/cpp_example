# mutex
add_executable(demo1_mutex demo1_mutex.cpp)
target_link_libraries(demo1_mutex PRIVATE Threads::Threads)

add_executable(demo2_recursive_mutex demo2_recursive_mutex.cpp)
target_link_libraries(demo2_recursive_mutex PRIVATE Threads::Threads)

add_executable(demo3_timed_mutex demo3_timed_mutex.cpp)
target_link_libraries(demo3_timed_mutex PRIVATE Threads::Threads)

add_executable(demo4_lock_guard demo4_lock_guard.cpp)
target_link_libraries(demo4_lock_guard PRIVATE Threads::Threads)

add_executable(demo5_unique_lock demo5_unique_lock.cpp)
target_link_libraries(demo5_unique_lock PRIVATE Threads::Threads)
