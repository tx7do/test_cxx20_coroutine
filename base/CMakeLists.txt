# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.0)

# 子目录下需要构建另一个工程
project(base)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量中
aux_source_directory(. DIR_SRCS)

#add_compile_options(${PROJECT_NAME} "-fcoroutines")

# 构建可执行文件
add_executable(
        ${PROJECT_NAME}
        ${DIR_SRCS}
)

# 指定目标链接的库
target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        pthread
        rt
)
