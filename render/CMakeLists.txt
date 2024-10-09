# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.0)

# 项目信息
project (CoolEngine)

# 查找指定目录下的所有源文件，然后将结果保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 设置使用C++标准版本
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

# 指定头文件
file(GLOB_RECURSE pipe_header_files  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )

# 指定生成目标
#add_executable(${PROJECT_NAME} ${C_FILES} ${CPP_FILES} ${H_FILES})
#add_executable(CoolEngine ${DIR_SRCS})
add_executable(CoolEngine  ${DIR_SRCS} ${pipe_header_files})