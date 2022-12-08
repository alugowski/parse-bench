/*
Copyright (C) 2019-2022 Adam Lugowski

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "parse_bench.h"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <charconv>
#include <sstream>

#include "fast_float/fast_float.h"

/**
 * Parse a block using istringstream
 *
 * The "default" C++ method.
 */
static void BlockParse_istringstream(benchmark::State& state) {
    std::size_t num_bytes = 0;

    int64_t row, col;
    double value;

    for ([[maybe_unused]] auto _ : state) {
        std::istringstream iss(kLineBlock);

        while (!iss.eof()) {
            iss >> row >> col >> value;

            benchmark::DoNotOptimize(row);
            benchmark::DoNotOptimize(col);
            benchmark::DoNotOptimize(value);
        }

        num_bytes += kLineBlock.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
}

BENCHMARK(BlockParse_istringstream)->Name("BlockParse/istringstream");

/**
 * Parse a block using from_chars and strtod
 */
static void BlockParse_from_chars_strtod(benchmark::State& state) {
    std::size_t num_bytes = 0;

    int64_t row, col;
    double value;

    for ([[maybe_unused]] auto _ : state) {
        errno = 0;

        const char* pos = kLineBlock.c_str();
        const char* end = pos + kLineBlock.size();

        while (pos != end && pos != nullptr) {

            std::from_chars_result row_result = std::from_chars(pos, end, row);
            if (row_result.ec != std::errc()) {
                break; // error testing
            }

            const char* col_start = row_result.ptr + std::strspn(row_result.ptr, " "); // skip separator

            std::from_chars_result col_result = std::from_chars(col_start, end, col);
            if (col_result.ec != std::errc()) {
                break; // error testing
            }

            // strtod does its own leading whitespace skipping
            char* value_end;
            value = std::strtod(col_result.ptr, &value_end);
            if (errno != 0) {
                break; // error checking
            }

            // find the newline
            pos = std::strchr(col_result.ptr, '\n');

            // bump to start of next line
            if (pos != end) {
                pos++;
            }

            benchmark::DoNotOptimize(row);
            benchmark::DoNotOptimize(col);
            benchmark::DoNotOptimize(value);
        }

        num_bytes += kLineBlock.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
}

BENCHMARK(BlockParse_from_chars_strtod)->Name("BlockParse/from_chars+strtod");

/**
 * Parse a block using from_chars. Parse floats using the fast_float version of from_chars for compiler compatibility.
 */
static void BlockParse_from_chars_ff(benchmark::State& state) {
    std::size_t num_bytes = 0;

    int64_t row, col;
    double value;

    for ([[maybe_unused]] auto _ : state) {
        errno = 0;

        const char* pos = kLineBlock.c_str();
        const char* end = pos + kLineBlock.size();

        while (pos != end && pos != nullptr) {

            std::from_chars_result row_result = std::from_chars(pos, end, row);
            if (row_result.ec != std::errc()) {
                break; // error testing
            }

            const char* col_start = row_result.ptr + std::strspn(row_result.ptr, " "); // skip separator

            std::from_chars_result col_result = std::from_chars(col_start, end, col);
            if (col_result.ec != std::errc()) {
                break; // error testing
            }

            const char* val_start = col_result.ptr + std::strspn(col_result.ptr, " "); // skip separator

            fast_float::from_chars_result val_result = fast_float::from_chars(val_start, end, value, fast_float::chars_format::general);
            if (val_result.ec != std::errc()) {
                break; // error testing
            }

            // find the newline
            pos = std::strchr(val_result.ptr, '\n');

            // bump to start of next line
            if (pos != end) {
                pos++;
            }

            benchmark::DoNotOptimize(row);
            benchmark::DoNotOptimize(col);
            benchmark::DoNotOptimize(value);
        }

        num_bytes += kLineBlock.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
}

BENCHMARK(BlockParse_from_chars_ff)->Name("BlockParse/from_chars+fast_float");
