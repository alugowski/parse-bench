/*
Copyright (C) 2019-2022 Adam Lugowski

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "parse_bench.h"

#include <cstdio>

/**
 * Tokenize line using strpbrk and strspn.
 */
static void LineTokenize_strpbrk(benchmark::State& state, const char* sep) {
    std::size_t num_bytes = 0;
    std::size_t num_lines = 0;

    const char *row_start, *row_end;
    const char *col_start, *col_end;
    const char *value_start, *value_end;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& line : kLines) {
            row_start = line.c_str();
            row_end = std::strpbrk(row_start, sep);
            if (!row_end) {
                break; // error testing
            }

            col_start = row_end + std::strspn(row_end, sep); // skip separator
            col_end = std::strpbrk(col_start, sep);
            if (!col_end) {
                break; // error testing
            }

            value_start = col_end + std::strspn(col_end, sep); // skip separator
            value_end = std::strpbrk(value_start, sep);
            if (value_end) {
                // value_end is always NULL in this benchmark
                break; // error testing
            }

            benchmark::DoNotOptimize(row_end);
            benchmark::DoNotOptimize(col_end);
            benchmark::DoNotOptimize(value_end);

            num_bytes += line.size();
        }
        num_lines += kLines.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["lines_tokenized_per_second"] = benchmark::Counter((double)num_lines, benchmark::Counter::kIsRate);
}

BENCHMARK_CAPTURE(LineTokenize_strpbrk, space_only, " ")->Name("LineTokenize/strpbrk/space_only");
BENCHMARK_CAPTURE(LineTokenize_strpbrk, space_tab, " \t")->Name("LineTokenize/strpbrk/space_tab");

/**
 * Tokenize line using strtok
 *
 * This benchmark serves to only provide a performance context because strtok is unusable:
 *  - strtok() is not thread safe.
 *  - strtok_r() is thread safe, but only available on POSIX systems. It is _not_ in the std namespace.
 *  - strtok_s() is standard C11, but Windows has an older incompatible version.
 */
static void LineTokenize_strtok(benchmark::State& state, const char* sep) {
    std::size_t num_bytes = 0;
    std::size_t num_lines = 0;

    // reserve space for a copy of the line because strtok makes modifications.
    std::size_t max_length = 0;
    for (const auto& line : kLines) {
        max_length = std::max(max_length, line.size());
    }
    std::vector<char> line_buffer(max_length + 1);
    char* line_copy = &line_buffer[0];

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& line : kLines) {
            // Copy the original string so that strtok can modify it
            // This copy is extra work that makes the method appear slightly slower but it's unavoidable.
            // This also highlights another drawback of strtok(): you can't use it on const arrays.
            std::strcpy(line_copy, line.c_str());

            char *row_s = std::strtok(line_copy, sep);
            if (!row_s) {
                break; // error testing
            }

            char *col_s = std::strtok(nullptr, sep);
            if (!col_s) {
                break; // error testing
            }

            char *val_s = std::strtok(nullptr, sep);
            if (!val_s) {
                break; // error testing
            }

            benchmark::DoNotOptimize(row_s);
            benchmark::DoNotOptimize(col_s);
            benchmark::DoNotOptimize(val_s);

            num_bytes += line.size();
        }

        num_lines += kLines.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["lines_tokenized_per_second"] = benchmark::Counter((double)num_lines, benchmark::Counter::kIsRate);
}

BENCHMARK_CAPTURE(LineTokenize_strtok, space_only, " ")->Name("LineTokenize/strtok/space_only");
BENCHMARK_CAPTURE(LineTokenize_strtok, space_tab, " \t")->Name("LineTokenize/strtok/space_tab");
