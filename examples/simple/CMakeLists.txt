cmake_minimum_required(VERSION 3.0)

project(simple)

add_executable(app1 app1.cpp)
target_link_libraries(app1 PRIVATE shmem)

add_executable(app2 app2.cpp)
target_link_libraries(app2 PRIVATE shmem)
