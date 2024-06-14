# 定义一个SRC_LIST变量，包含了该目录下的所有源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./redis REDIS_LIST)
# 指定生成的可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})
# 指定可执行文件链接时需要依赖的库文件
# target_link_libraries(ChatServer tiny_network  mysqlclient  pthread)

# 设置项目编译头文件搜索路径 -I
# include_directories(/home/mzy/Code/MpRpc/src/include)
include_directories(/usr/include/mprpc)
include_directories(/usr/local/include)  # 可能需要包含muduo的头文件

target_link_libraries(ChatServer mprpc protobuf zookeeper_mt   muduo_net muduo_base  mysqlclient hiredis pthread)
