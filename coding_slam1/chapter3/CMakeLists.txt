cmake_minimum_required(VERSION 3.17)
project(useSophus)

set(CMAKE_CXX_STANDARD 14)
find_package(Sophus REQUIRED)
include_directories( ${Sophus_INCLUDE_DIRS} )

add_executable(useSophus useSophus.cpp)
target_link_libraries(useSophus ${Sophus_LIBRARIES} )
