cmake_minimum_required(VERSION 3.17)
project(useEigen)

set(CMAKE_CXX_STANDARD 14)
set( CMAKE_BUILD_TYPE "Release" )
set( CMAKE_CXX_FLAGS "-O3" )

find_package(Eigen3 3.3 REQUIRED)

add_executable(useEigen eigenMatrix.cpp)

