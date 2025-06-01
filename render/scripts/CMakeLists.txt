# #cmake_minimum_required(VERSION 3.10) # CMake 最低版本号要求 建议使用较新的CMake版本

# #project (CoolEngine CXX) # 定义项目名称和使用的语言


# # 设置C++标准
# set(CMAKE_CXX_STANDARD 17) # 或者你需要的任何C++标准，例如11, 14, 20
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF) # 禁用编译器扩展，强制遵循标准

# # 查找指定目录下的所有源文件，然后将结果保存到 DIR_SRCS 变量
# aux_source_directory(. DIR_SRCS)

# # 添加头文件搜索路径
# # 这里使用相对路径，因为CMakeLists.txt在scripts目录下
# # 而include目录也在scripts目录下
# # target_include_directories(CoolEngine PUBLIC include)

# # 查找所有.cpp源文件
# file(GLOB_RECURSE SOURCES "*.cpp")

# # 指定头文件
#  file(GLOB_RECURSE pipe_header_files  $../{CMAKE_CURRENT_SOURCE_DIR}/include/*.h )

# # 指定生成目标
# #add_executable(${PROJECT_NAME} ${C_FILES} ${CPP_FILES} ${H_FILES})
# #add_executable(CoolEngine ${DIR_SRCS})
#  #add_executable(CoolEngine  ${DIR_SRCS} ${pipe_header_files})


#  add_executable(CoolEngine ${SOURCES})


# E:\github\CoolEngine\render\scripts\CMakeLists.txt

# 这里不需要再次调用 project()，因为已经在顶层 CMakeLists.txt 中定义了项目。

# 定义可执行文件的名称
set(EXECUTABLE_NAME CoolEngineExecutable)

# # 查找指定目录下的所有源文件，然后将结果保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 查找所有 .cpp 源文件
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# 查找 pipe_header_files
file(GLOB_RECURSE pipe_header_files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

source_group("Header Files" FILES ${pipe_header_files})

# 如果你需要将这些头文件也添加到源文件列表中进行编译（通常不需要）
# list(APPEND SOURCES ${pipe_header_files})




# 先创建目标 (add_executable)，然后才能配置它
# 步骤 1: 创建可执行文件目标
# CoolEngineExecutable 将是生成的可执行文件的名称
# 确保你的 SOURCES 列表中包含一个 main 函数的 .cpp 文件
# add_executable(${EXECUTABLE_NAME} ${SOURCES})
# add_executable(${EXECUTABLE_NAME} ${SOURCES}${pipe_header_files})

#  格式：add_executable(${PROJECT_NAME} ${C_FILES} ${CPP_FILES} ${H_FILES})
# 不加${H_FILES} 则VS工程没头文件夹

add_executable(${EXECUTABLE_NAME}   ${DIR_SRCS} ${pipe_header_files})

target_include_directories(${EXECUTABLE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# =====================================================================

# 如果你的可执行文件需要链接到其他库，可以在这里添加
# target_link_libraries(${EXECUTABLE_NAME} YourOtherLibrary)