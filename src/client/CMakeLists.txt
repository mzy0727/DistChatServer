# 定义了一个SRC_LIST变量，包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(../server/protobuf PROTOBUF_LIST)
# 指定生成可执行文件
add_executable(ChatClient ${SRC_LIST} ${PROTOBUF_LIST})
# 指定可执行文件链接时需要依赖的库文件
target_link_libraries(ChatClient mprpc protobuf zookeeper_mt  pthread  muduo_net muduo_base  mysqlclient hiredis)