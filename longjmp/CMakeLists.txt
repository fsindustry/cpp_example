# longjmp
add_executable(demo1_longjmp_basic demo1_longjmp_basic.c)
add_executable(demo2_call_later_setjmp demo2_call_later_setjmp.c)
add_executable(demo3_longjmp_cross_threads demo3_longjmp_cross_threads.c)
target_link_libraries(demo3_longjmp_cross_threads PRIVATE Threads::Threads)
add_executable(demo4_longjmp_stack_recovery demo4_longjmp_stack_recovery.c)
add_executable(demo5_siglongjmp_basic demo5_siglongjmp_basic.c)



