cmake_minimum_required(VERSION 3.19)
project(parse_bench VERSION 1.0.0)
SET(CMAKE_CXX_STANDARD 20)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 -DNDEBUG")
endif()

# Add Google Benchmark
include(cmake/GoogleBenchmark.cmake)

# Add fast_float library
include(cmake/fast_float.cmake)

# Test for float version of std::from_chars.
# The method is specified in C++17, but compiler support is spotty.
include(CheckSourceCompiles)
check_source_compiles(CXX "
#include <charconv>
int main(void) {
    double value = 0;
    const char* ptr;
    std::from_chars_result result = std::from_chars(ptr, ptr, value, std::chars_format::general);
    return 0;
}
" float_from_chars_supported)
if (float_from_chars_supported)
    add_definitions(-DFROM_CHARS_DOUBLE_SUPPORTED)
else()
    message("Float version of std::from_chars not detected. Omitting its test.")
endif()


# benchmark string parsing implementations
add_executable(parse-bench bench_stream.cpp bench_splitlines.cpp bench_line_tokenize.cpp bench_parse_int.cpp bench_parse_double.cpp bench_line_parse.cpp bench_block_parse.cpp bench_block_parallel_parse.cpp main.cpp)
target_link_libraries(parse-bench benchmark::benchmark fast_float)

# enable OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message("OpenMP found")
    add_definitions(-DENABLE_OPENMP)
    target_link_libraries(parse-bench OpenMP::OpenMP_CXX)
else()
    message("OpenMP not found, parallel benchmarks disabled.")
endif()

