#!/bin/bash

# Build
cmake -S . -B cmake-build-release/ -D CMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release/ --target parse-bench


# run
cmake-build-release/parse-bench --benchmark_out_format=json --benchmark_out=../plots/benchmark_outputs/C++.json