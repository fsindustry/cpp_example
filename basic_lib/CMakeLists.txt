# 加载所有的源码，和makefile wildcard类似
# 等价于 SET(DIR_SRCS dir1.c  dir12.c)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

message("源文件列表: ${DIR_SRCS}")

# 默认是静态库
# 注： c项目和cpp项目识别的源文件后缀不同
# 使用c源文件时，或者改后缀，或者使用extern "C"声明
ADD_LIBRARY(basic_lib ${DIR_SRCS})