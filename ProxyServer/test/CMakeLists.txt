cmake_minimum_required(VERSION 3.0)
project(test)

# 配置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


include_directories(/home/mzy/Code/ChatServer/thirdparty/)

# 指定生成的可执行文件
add_executable(testspdlog testspdlog.cpp)
