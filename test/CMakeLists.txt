cmake_minimum_required(VERSION 3.14)

project(test)

set(INSTALL_GTEST OFF)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(shmem_test shmem_test.cpp)
target_link_libraries(shmem_test PRIVATE shared_mem GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(shmem_test)
