cmake_minimum_required(VERSION 3.0)


SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(shmem)


add_library(shmem STATIC shmem.cpp)
target_include_directories(shmem PUBLIC .)

add_executable(app1 app1.cpp)
target_link_libraries(app1 PRIVATE shmem)

add_executable(app2 app2.cpp)
target_link_libraries(app2 PRIVATE shmem)
