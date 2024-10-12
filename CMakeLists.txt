cmake_minimum_required(VERSION 3.0)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)
SET(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(shared_mem)

set(BOOST_INCLUDE_LIBRARIES interprocess)
set(BOOST_ENABLE_CMAKE ON)

include(cmake/FetchBoost.cmake)

set(SOURCES
    src/shared_mem.cpp
)

add_library(shared_mem STATIC ${SOURCES})
target_include_directories(shared_mem PUBLIC include Boost::interprocess)

add_subdirectory(examples)
add_subdirectory(test)
