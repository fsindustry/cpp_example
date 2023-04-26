
# atomic
add_executable(demo1_atomic_quickstart demo1_atomic_quickstart.cpp)
target_link_libraries(demo1_atomic_quickstart PRIVATE Threads::Threads)
