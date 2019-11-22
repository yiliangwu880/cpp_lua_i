# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
set(project_name "sample")
# 项目信息
project (${project_name} CXX)


# include directories
#INCLUDE_DIRECTORIES(after  ./cpp_cfg/com/)


# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
file(GLOB_RECURSE SRCS  ${PROJECT_SOURCE_DIR} "*.cpp")
file(GLOB_RECURSE SRCS2  ${PROJECT_SOURCE_DIR} "../src/*.cpp")

# 指定生成目标
ADD_EXECUTABLE(${project_name} ${SRCS} ${SRCS2})
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/sample)

# 设置链接库
target_link_libraries(${project_name} liblua.a dl)

