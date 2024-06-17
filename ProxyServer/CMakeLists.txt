cmake_minimum_required(VERSION 3.0)
project(chat)

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
# 配置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/include/server/rpcservice)
include_directories(${PROJECT_SOURCE_DIR}/include/server/protobuf)
include_directories(${PROJECT_SOURCE_DIR}/include/client)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)


# # 加载tiny_network的相关头文件
# include_directories(/usr/local/include/tiny_network/net)
# include_directories(/usr/local/include/tiny_network/base)
# include_directories(/usr/local/include/tiny_network/timer)
# include_directories(/usr/local/include/tiny_network/base)
# include_directories(/usr/local/include/tiny_network/logger)
# 设置项目编译头文件搜索路径 -I
include_directories(/usr/include/mprpc)
include_directories(/usr/local/include)  # 可能需要包含muduo的头文件
# 设置项目库文件搜索路径 -L
link_directories(/usr/local/lib)

# 加载子目录
add_subdirectory(src)