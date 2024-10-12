cmake_minimum_required(VERSION 3.0)


SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)
SET(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(shmem)

set(SOURCES
    src/shmem.cpp
)

add_library(shmem STATIC ${SOURCES})
target_include_directories(shmem PUBLIC include)

add_subdirectory(examples)
add_subdirectory(test)
