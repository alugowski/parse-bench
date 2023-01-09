/*
Copyright (C) 2019-2022 Adam Lugowski

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "parse_bench.h"

#include <cstring>
#include <sstream>

/**
 * Find line breaks using strchr
 */
static void SplitLines_strchr(benchmark::State& state) {
    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        const char* pos = kLineBlock.c_str();
        do {
            pos = std::strchr(pos + 1, '\n');
            benchmark::DoNotOptimize(pos);
        } while (pos != nullptr);

        num_bytes += kLineBlock.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
}

BENCHMARK(SplitLines_strchr)->Name("SplitLines/strchr");

/**
 * Find line breaks using string::find_first_of (needle is single character variant)
 */
static void SplitLines_find_first_of_char(benchmark::State& state) {
    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        size_t pos = 0;
        do {
            pos = kLineBlock.find_first_of('\n', pos + 1);
            benchmark::DoNotOptimize(pos);
        } while (pos != std::string::npos);

        num_bytes += kLineBlock.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
}

BENCHMARK(SplitLines_find_first_of_char)->Name("SplitLines/find_first_of(char)");

/**
 * Find line breaks using string::find_first_of (needle is string variant)
 */
static void SplitLines_find_first_of_str(benchmark::State& state) {
    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        size_t pos = 0;
        do {
            pos = kLineBlock.find_first_of("\r\n", pos + 1);
            benchmark::DoNotOptimize(pos);
        } while (pos != std::string::npos);

        num_bytes += kLineBlock.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
}

BENCHMARK(SplitLines_find_first_of_str)->Name("SplitLines/find_first_of(str)");

/**
 * Find lines using std::getline()
 */
static void SplitLines_getline(benchmark::State& state) {
    std::size_t num_bytes = 0;

    for ([[maybe_unused]] auto _ : state) {
        std::istringstream instream{kLineBlock};
        while (!instream.eof()) {
            std::string line;
            std::getline(instream, line);

            benchmark::DoNotOptimize(line);
        }

        num_bytes += kLineBlock.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
}

BENCHMARK(SplitLines_getline)->Name("SplitLines/getline");