/*
Copyright (C) 2019-2022 Adam Lugowski

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef ENABLE_OPENMP
#include "parse_bench.h"

#include <charconv>
#include <sstream>
#include <omp.h>

// <cstdio> does not include fmemopen which is needed for the scanf test
#include <stdio.h> // NOLINT(modernize-deprecated-headers)


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
 * Parse a block using io streams.
 */
void ParseChunk_istringstream(const char* pos, size_t length) {
    int64_t row, col;
    double value;

    std::istringstream iss(std::string(pos, length));

    while (!iss.eof()) {
        iss >> row >> col >> value;

        benchmark::DoNotOptimize(row);
        benchmark::DoNotOptimize(col);
        benchmark::DoNotOptimize(value);
    }
}

/**
 * Parse a block using fscanf.
 *
 * Note that sscanf is a poor choice for parsing large in-memory strings. Implementations tend to want to reuse
 * their internal vfscanf() implementation, so convert the string into a FILE object before hand. This can mean not only
 * a copy but a strlen on every sscanf call. When parsing a large chunk, that means every line.
 *
 * Here we attempt to do that conversion once for the entire chunk and use fscanf.
 *
 * See https://stackoverflow.com/a/23924112
 */
void ParseChunk_fscanf(const char* pos, size_t length) {
    int64_t row, col;
    double value;

    // open the string as a FILE object
    FILE *chunk = fmemopen(const_cast<char *>(pos), length, "r");

    while (fscanf(chunk, "%lld %lld %lf\n", &row, &col, &value) == 3) { // NOLINT(cert-err34-c)
        benchmark::DoNotOptimize(row);
        benchmark::DoNotOptimize(col);
        benchmark::DoNotOptimize(value);
    }

    fclose(chunk);
}

/**
 * Parse a block in parallel.
 *
 * First benchmark argument determines the block parse implementation.
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
static void BlockParseParallel_multi(benchmark::State& state) {
    const int whichImpl = (int)state.range(0);
    const int numThreads = (int)state.range(1);
    const int chunkSize = (int)state.range(2);

    size_t target_bytes = 200u << 20u;
    if (whichImpl == 3) {
        // scanf is slow
        target_bytes = 20u << 20u;
    }
    if (chunkSize > 8u << 20u && numThreads > 6) {
        // use a larger problem to reduce uneven cpu/task counts
        target_bytes = 400u << 20u;
    }

    std::size_t num_bytes = 0;
    const std::string kBigLineBlock = ConstructManyLines(target_bytes);
    omp_set_num_threads(numThreads);

    for ([[maybe_unused]] auto _ : state) {
        // chunk the input
        auto chunks = getChunks(kBigLineBlock, chunkSize);

        // process chunks in parallel
        switch (whichImpl) {
            case 0: // from_chars with fast_float
#pragma omp parallel for default(none) shared(chunks)
                for (auto chunk : chunks) {
                    ParseChunk_from_chars_ff(chunk.start, chunk.length);
                }
                break;

            case 1: // from_chars with strtod
#pragma omp parallel for default(none) shared(chunks)
                for (auto chunk : chunks) {
                    ParseChunk_from_chars_strtod(chunk.start, chunk.length);
                }
                break;

            case 2: // istringstream
#pragma omp parallel for default(none) shared(chunks)
                for (auto chunk : chunks) {
                    ParseChunk_istringstream(chunk.start, chunk.length);
                }
                break;

            case 3: // sscanf
#pragma omp parallel for default(none) shared(chunks)
                for (auto chunk : chunks) {
                    ParseChunk_fscanf(chunk.start, chunk.length);
                }
                break;

            default:
                break;
        }

        num_bytes += kBigLineBlock.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["p"] = benchmark::Counter((double)numThreads);
    state.counters["chunk_size"] = benchmark::Counter((double)chunkSize, benchmark::Counter::kDefaults, benchmark::Counter::kIs1024);
}

BENCHMARK(BlockParseParallel_multi)
        ->Name("BlockParseParallel/from_chars(fast_float)")
        ->UseRealTime()
        ->ArgsProduct({
                              {0}, // from_chars(fast_float) version
                              {1, 2, 3, 4, 5, 6, 7, 8}, // number of threads
                              {1u << 10u, 1u << 20u, 10u << 20u} // chunk sizes
                      });


BENCHMARK(BlockParseParallel_multi)
        ->Name("BlockParseParallel/from_chars+strtod")
        ->UseRealTime()
        ->ArgsProduct({
                              {1}, // from_chars+strtod version
                              {1, 2, 3, 4, 5, 6, 7, 8}, // number of threads
                              {1u << 20u} // chunk sizes
                      });

BENCHMARK(BlockParseParallel_multi)
        ->Name("BlockParseParallel/istringstream")
        ->UseRealTime()
        ->ArgsProduct({
                              {2}, // istringstream version
                              {1, 2, 3, 4, 5, 6, 7, 8}, // number of threads
                              {1u << 20u} // chunk sizes
                      });

BENCHMARK(BlockParseParallel_multi)
        ->Name("BlockParseParallel/scanf")
        ->UseRealTime()
        ->ArgsProduct({
                              {3}, // scanf version
                              {1, 2, 3, 4, 5, 6, 7, 8}, // number of threads
                              {1u << 20u} // chunk sizes
                      });


#endif // ENABLE_OPENMP