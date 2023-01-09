/*
Copyright (C) 2019-2022 Adam Lugowski

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "parse_bench.h"

#include <numeric>

const std::array<std::string, 3> kLines = {
    "123456 234567 333.323",
    "1 234567 333.323",
    "1 2 3",
};

// keep in sync with python/bench_parse.py
const std::array<std::string, 2> kIntStrings = {
    "123456",
    "1",
};

const std::array<std::string, 3> kDoubleStrings = {
    "123456",
    "1",
    "333.323",
};

/**
 * Constructs a large string block composed of repeated lines from kLines.
 *
 * @param byte_target size in bytes of the result
 */
std::string ConstructManyLines(std::size_t byte_target) {
    std::vector<char> chunk;
    for (const auto& line : kLines) {
        std::copy(std::begin(line), std::end(line), std::back_inserter(chunk));
        chunk.emplace_back('\n');
    }

    std::vector<char> result;
    result.reserve(byte_target + chunk.size() + 1);

    while (result.size() < byte_target) {
        std::copy(std::begin(chunk), std::end(chunk), std::back_inserter(result));
    }
    result.emplace_back(0);

    return {result.data()};
}

/**
 * Large string with many lines.
 */
const std::string kLineBlock = ConstructManyLines(50u << 20u);

BENCHMARK_MAIN();
