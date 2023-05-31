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

# Add Google double-conversion library
include(cmake/double_conversion.cmake)

# Add Cpp Alliance's charconv library
include(cmake/cppalliance_charconv.cmake)

# Add Dragonbox floating-point formatting library
include(cmake/dragonbox.cmake)

# Add Ryu library
include(cmake/ryu.cmake)

# benchmark string parsing implementations
add_executable(parse-bench
        bench_stream.cpp
        bench_splitlines.cpp
        bench_line_tokenize.cpp
        bench_parse_int.cpp
        bench_parse_double.cpp
        bench_format_double.cpp
        bench_line_parse.cpp
        bench_block_parse.cpp
        bench_block_parallel_parse.cpp
        main.cpp)
target_link_libraries(parse-bench
        benchmark::benchmark
        fast_float
        double-conversion
        Boost::charconv
        dragonbox::dragonbox_to_chars
        ryu::ryu)

# find compiler identification string
set(COMPILER_STR "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message("Compiler: ${COMPILER_STR}")
target_compile_definitions(parse-bench PUBLIC COMPILER_STR="${COMPILER_STR}")

# Test for float versions of std::from_chars and std::to_chars.
# These methods is specified in C++17, but compiler support is only recently catching up.
include(cmake/from_chars_tests.cmake)
include(cmake/to_chars_tests.cmake)

if (from_chars_double_supported)
    message("Double version of std::from_chars detected.")
    target_compile_definitions(parse-bench PUBLIC FROM_CHARS_DOUBLE_SUPPORTED)
else()
    message("Double version of std::from_chars not detected. Omitting its test.")
endif()

if (to_chars_double_supported)
    message("Double version of std::to_chars detected.")
    target_compile_definitions(parse-bench PUBLIC TO_CHARS_DOUBLE_SUPPORTED)
else()
    message("Double version of std::to_chars not detected. Omitting its test.")
endif()

# enable OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message("OpenMP found")
    add_definitions(-DENABLE_OPENMP)
    target_link_libraries(parse-bench OpenMP::OpenMP_CXX)
else()
    message("OpenMP not found, parallel benchmarks disabled.")
endif()

