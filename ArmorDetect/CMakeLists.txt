cmake_minimum_required(VERSION 3.17)

project(test)

#查找opencv
find_package(OpenCV REQUIRED)

#包含头文件路径
include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

#添加可执行文件

file(GLOB_RECURSE SRC_FILES src/*cpp)

add_executable(test ${SRC_FILES} main.cpp)

#链接库
target_link_libraries(test ${OpenCV_LIBS})