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
#include <sstream>

#if defined(__cplusplus) && __cplusplus >= 201703L
#include <charconv>
#endif

/**
 * Parse a line using istringstream
 */
static void LineParse_istringstream(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_lines = 0;

    int64_t row, col;
    double value;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& line : kLines) {
            std::istringstream iss(line);
            iss >> row >> col >> value;

            benchmark::DoNotOptimize(row);
            benchmark::DoNotOptimize(col);
            benchmark::DoNotOptimize(value);

            num_bytes += line.size();
        }
        num_lines += kLines.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["lines_parsed_per_second"] = benchmark::Counter((double)num_lines, benchmark::Counter::kIsRate);
}

BENCHMARK(LineParse_istringstream)->Name("LineParse/istringstream");

/**
 * Parse a line using sscanf
 */
static void LineParse_sscanf(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_lines = 0;

    int64_t row, col;
    double value;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& line : kLines) {
            std::sscanf(line.c_str(), "%lld %lld %lf", &row, &col, &value); // NOLINT(cert-err34-c)

            benchmark::DoNotOptimize(row);
            benchmark::DoNotOptimize(col);
            benchmark::DoNotOptimize(value);

            num_bytes += line.size();
        }
        num_lines += kLines.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["lines_parsed_per_second"] = benchmark::Counter((double)num_lines, benchmark::Counter::kIsRate);
}

BENCHMARK(LineParse_sscanf)->Name("LineParse/sscanf");

/**
 * Parse a line using strtoll and strtod.
 *
 * On error strtoll returns 0 or causes undefined behavior.
 */
static void LineParse_strtoll_strtod(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_lines = 0;

    int64_t row, col;
    double value;

    errno = 0;
    for ([[maybe_unused]] auto _ : state) {
        for (const auto& line : kLines) {
            char *end;

            row = std::strtoll(line.c_str(), &end, 10);
            col = std::strtoll(end, &end, 10);
            value = std::strtod(end, nullptr);

            if (errno != 0) {
                break; // error checking
            }

            benchmark::DoNotOptimize(row);
            benchmark::DoNotOptimize(col);
            benchmark::DoNotOptimize(value);

            num_bytes += line.size();
        }
        num_lines += kLines.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["lines_parsed_per_second"] = benchmark::Counter((double)num_lines, benchmark::Counter::kIsRate);
}

BENCHMARK(LineParse_strtoll_strtod)->Name("LineParse/strtoll+strtod");

/**
 * Parse line using from_chars for ints and strtod for doubles.
 */
static void LineParse_from_chars_strtod(benchmark::State& state, const char* sep) {
    std::size_t num_bytes = 0;
    std::size_t num_lines = 0;

    int64_t row, col;
    double value;

    errno = 0;
    for ([[maybe_unused]] auto _ : state) {
        for (const auto& line : kLines) {
            const char* line_end = line.data() + line.size();

            std::from_chars_result row_result = std::from_chars(line.data(), line_end, row);
            if (row_result.ec != std::errc()) {
                break; // error testing
            }

            const char* col_start = row_result.ptr + std::strspn(row_result.ptr, sep); // skip separator

            std::from_chars_result col_result = std::from_chars(col_start, line_end, col);
            if (col_result.ec != std::errc()) {
                break; // error testing
            }

            // strtod does its own leading whitespace skipping
            value = std::strtod(col_result.ptr, nullptr);
            if (errno != 0) {
                break; // error checking
            }

            benchmark::DoNotOptimize(row);
            benchmark::DoNotOptimize(col);
            benchmark::DoNotOptimize(value);

            num_bytes += line.size();
        }
        num_lines += kLines.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["lines_parsed_per_second"] = benchmark::Counter((double)num_lines, benchmark::Counter::kIsRate);
}

BENCHMARK_CAPTURE(LineParse_from_chars_strtod, space_only, " ")->Name("LineParse/from_chars+strtod/space_only");
BENCHMARK_CAPTURE(LineParse_from_chars_strtod, space_tab, " \t")->Name("LineParse/from_chars+strtod/space_tab");

