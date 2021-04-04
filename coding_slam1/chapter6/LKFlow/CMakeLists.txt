cmake_minimum_required(VERSION 3.10)
project(LKFlow)

set( CMAKE_BUILD_TYPE Release )
set( CMAKE_CXX_FLAGS "-std=c++11 -O3" )

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(LKFlow useLK.cpp)
target_link_libraries(LKFlow ${OpenCV_LIBS})

