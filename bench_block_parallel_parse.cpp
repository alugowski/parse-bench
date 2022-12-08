/*
Copyright (C) 2019-2022 Adam Lugowski

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef ENABLE_OPENMP
#include "parse_bench.h"

#include <cstdio>
#include <charconv>
#include <omp.h>


struct Chunk {
    const char* start;
    size_t length;

    Chunk(const char *start, size_t length) : start(start), length(length) {}
};

static size_t getNextChunkEnd(const std::string& block, const size_t pos, const int chunkSize) {
    size_t end = block.find_first_of('\n', pos + chunkSize);

    if (end == std::string::npos) {
        return block.size();
    } else {
        return end;
    }
}

/**
 * Split a large string block into chunks on line boundaries.
 */
std::vector<Chunk> getChunks(const std::string& block, const int chunkSize) {
    std::vector<Chunk> ret;

    for (size_t pos = 0; pos < block.size(); ) {
        size_t end = getNextChunkEnd(block, pos, chunkSize);
        size_t length = end - pos;

        ret.emplace_back(block.c_str() + pos, length);
        pos = end + 1;
    }

    return ret;
}

/**
 * Parse a block using from_chars (fast_float version for floats), in parallel.
 *
 * The model is that IO is done by a single thread, then the bytes are split into chunks and the chunks parsed in
 * parallel. The splitting is timed.
 *
 * Assumptions:
 * Chunks can be processed in any order. Any ordering requirements would result in a performance hit.
 *
 * Chunk boundaries can be determined locally, namely by looking for newlines.
 *
 * Exact line numbers are not required. If required, such as for good error messages, then the chunking process would
 * require a scan and a performance hit.
 */
static void BlockParseParallel_from_chars_ff(benchmark::State& state) {
    std::size_t num_bytes = 0;
    const std::string kBigLineBlock = ConstructManyLines(400u << 20u);
    omp_set_num_threads((int)state.range(0));

    for ([[maybe_unused]] auto _ : state) {
        // chunk the input
        auto chunks = getChunks(kBigLineBlock, (int)state.range(1));

        // process chunks in parallel
#pragma omp parallel for default(none) shared(chunks)
        for (auto chunk : chunks) {
            ParseChunk_from_chars_ff(chunk.start, chunk.length);
        }

        num_bytes += kBigLineBlock.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["p"] = benchmark::Counter((double)state.range(0));
    state.counters["chunk_size"] = benchmark::Counter((double)state.range(1), benchmark::Counter::kDefaults, benchmark::Counter::kIs1024);
}

BENCHMARK(BlockParseParallel_from_chars_ff)
        ->Name("BlockParseParallel/from_chars(fast_float)")
        ->UseRealTime()
        ->ArgsProduct({
            {1, 2, 3, 4, 5, 6, 7, 8}, // number of threads
            {1u << 10u, 1u << 20u, 10u << 20u} // chunk sizes
        });

/**
 * Same as BlockParseParallel_from_chars_ff except using strtod instead of from_chars.
 *
 * The main takeaway here is that strtod has internal locking on some platforms that hurts parallel performance.
 */
static void BlockParseParallel_from_chars_strtod(benchmark::State& state) {
    std::size_t num_bytes = 0;
    const std::string kBigLineBlock = ConstructManyLines(200u << 20u);
    omp_set_num_threads((int)state.range(0));

    for ([[maybe_unused]] auto _ : state) {
        // chunk the input
        auto chunks = getChunks(kBigLineBlock, (int)state.range(1));

        // process chunks in parallel
#pragma omp parallel for default(none) shared(chunks)
        for (auto chunk : chunks) {
            ParseChunk_from_chars_strtod(chunk.start, chunk.length);
        }

        num_bytes += kBigLineBlock.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["p"] = benchmark::Counter((double)state.range(0));
    state.counters["chunk_size"] = benchmark::Counter((double)state.range(1), benchmark::Counter::kDefaults, benchmark::Counter::kIs1024);
}
BENCHMARK(BlockParseParallel_from_chars_strtod)
        ->Name("BlockParseParallel/from_chars+strtod")
        ->UseRealTime()
        ->ArgsProduct({
            {1, 2, 3, 4, 5, 6, 7, 8}, // number of threads
            {1u << 20u} // chunk sizes
        });

#endif // ENABLE_OPENMP