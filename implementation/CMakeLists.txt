cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wstrict-aliasing -Wstrict-aliasing=2 -fconcepts")
set (CMAKE_CXX_FLAGS_DEBUG "-Og -g -fsanitize=undefined,address")
set (CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")

project (AnyIteratorTests)

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB tests ${PROJECT_SOURCE_DIR}/tests/*.t.cpp)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(AnyIteratorTests ${tests})
target_link_libraries(AnyIteratorTests ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

project (AnyIteratorBenchmarks)

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB benchmarks ${PROJECT_SOURCE_DIR}/benchmarks/*.cpp)

find_package(Threads REQUIRED)
find_package(benchmark REQUIRED)
include_directories(${benchmark_INCLUDE_DIRS})

add_executable(AnyIteratorBenchmarks ${benchmarks})
target_link_libraries(AnyIteratorBenchmarks benchmark ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS AnyIteratorTests AnyIteratorBenchmarks DESTINATION bin)
