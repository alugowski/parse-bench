cmake_minimum_required(VERSION 3.11)
project(parse_bench VERSION 1.0.0)
SET(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Add Google Benchmark
include(cmake/GoogleBenchmark.cmake)

# Add fast_float library
include(cmake/fast_float.cmake)

# benchmark string parsing implementations
add_executable(parse-bench bench_stream.cpp bench_splitlines.cpp bench_line_tokenize.cpp bench_parse_int.cpp bench_parse_double.cpp bench_line_parse.cpp bench_block_parse.cpp main.cpp)
target_link_libraries(parse-bench benchmark::benchmark fast_float)
