cmake_minimum_required(VERSION 3.0)
project(main)

# 配置编译选项 有了这个编译出来的文件就是可调式的？
set(CMake_CXX_FLAGS ${CMake_CXX_FLAGS} -g)

# 配置头文件搜索路径

# 配置库文件搜索路径

# 设置需要编译的源文件列表
set(SRC_LIST ./muduoTest.cpp)

# 生成可执行文件、库文件 
# add_executable(server muduoTest.cpp)
# 或者这样写
add_executable(server ${SRC_LIST})

# 设置最终可执行文件的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# add_library(server muduoTest.cpp)

# 把 . 指定路径下的所有源文件名字放入变量名
# aux_source_directory(. SRC_LIST)

# 设置需要链接的库文件
target_link_libraries(server muduo_net muduo_base pthread)


