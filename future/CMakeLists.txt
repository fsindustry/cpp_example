# future
add_executable(demo1_future demo1_future.cpp)
target_link_libraries(demo1_future PRIVATE Threads::Threads)

add_executable(demo2_package_task demo2_package_task.cpp)
target_link_libraries(demo2_package_task PRIVATE Threads::Threads)

add_executable(demo3_promise demo3_promise.cpp)
target_link_libraries(demo3_promise PRIVATE Threads::Threads)
