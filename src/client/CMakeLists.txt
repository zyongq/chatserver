# 定义了一个SRC_LIST变量，包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)

# 指定生成可执行文件
add_executable(ChatClient ${SRC_LIST})

# 指定可执行文件链接时需要依赖的库文件
# 有读写两个线程，要使用pthread
# 得到的基于控制台的客户 而非UI界面，需要UI界面可以考虑使用Qt
target_link_libraries(ChatClient pthread)