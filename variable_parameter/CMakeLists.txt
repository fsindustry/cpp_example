
set(CMAKE_C_FLAGS -Wno-unused-value)
set(CMAKE_CXX_FLAGS -Wno-unused-value)

# variable parameter
add_executable(demo1_var_param demo1_var_param.cpp)
add_executable(demo2_var_param_by_recursive demo2_var_param_by_recursive.cpp)
add_executable(demo3_var_param_by_comma demo3_var_param_by_comma.cpp)
add_executable(demo4_var_param_forward demo4_var_param_forward.cpp)
