# 指定CMake的最低版本
cmake_minimum_required(VERSION 3.10)

# 定义项目名称
# 这个名称通常会成为Visual Studio解决方案的名称
project(CoolEngine CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加 'scripts' 子目录
# CMake 会进入 E:/github/CoolEngine/render/scripts 目录，
# 并在其中寻找并处理 CMakeLists.txt 文件。
add_subdirectory(scripts)

# 如果你的可执行文件最终需要安装，可以在这里添加安装规则
# install(TARGETS CoolEngineExecutable DESTINATION bin)
