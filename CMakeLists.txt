cmake_minimum_required(VERSION 3.0)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)
SET(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(shared_mem)

include(cmake/FetchBoost.cmake)

set(SOURCES
    src/shared_mem.cpp
)

if(UNIX)
    set(RT_LIBS rt)
endif()

add_library(shared_mem STATIC ${SOURCES})
target_include_directories(shared_mem PUBLIC include Boost::interprocess)
target_link_libraries(shared_mem PUBLIC ${RT_LIBS})

add_subdirectory(examples)
add_subdirectory(test)
