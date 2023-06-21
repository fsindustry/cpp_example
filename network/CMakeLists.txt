add_executable(demo1_socket demo1_socket.cpp)
target_link_libraries(demo1_socket PRIVATE Threads::Threads)

add_executable(demo2_select demo2_select.cpp)
add_executable(demo3_epoll demo3_epoll.cpp)
add_executable(demo4_c1000k_server demo4_c1000k_server.cpp)
add_executable(demo4_c1000k_client demo4_c1000k_client.cpp)


message(WARNING "home dir: ${CMAKE_HOME_DIRECTORY}")
message(WARNING "source dir: ${CMAKE_SOURCE_DIR}")


# echo server demo
add_executable(d1_echo_client echo/d1_echo_client.cpp)
target_link_libraries(d1_echo_client basic_lib)

add_executable(d1_multi_thread_echo_server echo/d1_multi_thread_echo_server.cpp)
target_link_libraries(d1_multi_thread_echo_server Threads::Threads basic_lib)

add_executable(d2_multi_process_echo_server echo/d2_multi_process_echo_server.cpp)
target_link_libraries(d2_multi_process_echo_server Threads::Threads basic_lib)


