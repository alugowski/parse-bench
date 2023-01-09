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
#include <charconv>

#if defined(__cplusplus) && __cplusplus >= 201703L
/**
 * Convert a single field from string to int.
 */
static void IntFieldParse_from_chars(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kIntStrings) {
            int64_t value;
            std::from_chars_result result = std::from_chars(field.data(), field.data() + field.size(), value);
            if (result.ec != std::errc()) {
                break; // error testing
            }
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kIntStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(IntFieldParse_from_chars)->Name("IntFieldParse/from_chars");
#endif

/**
 * Convert a single field from string to int using atol.
 *
 * atol() should be avoided because it does not report conversion errors. It is only included here for
 * a performance context.
 */
static void IntFieldParse_atol(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kIntStrings) {
            long value = std::atol(field.c_str()); // NOLINT(cert-err34-c)
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kIntStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(IntFieldParse_atol)->Name("IntFieldParse/atol");

/**
 * Convert a single field from string to int.
 */
static void IntFieldParse_stoll(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    errno = 0;
    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kIntStrings) {
            int64_t value = std::stoll(field, nullptr, 10);
            if (errno != 0) {
                break; // error checking
            }
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kIntStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(IntFieldParse_stoll)->Name("IntFieldParse/stoll");

/**
 * Convert a single field from string to int.
 */
static void IntFieldParse_strtoll(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    errno = 0;
    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kIntStrings) {
            int64_t value = std::strtoll(field.c_str(), nullptr, 10);
            if (errno != 0) {
                break; // error checking
            }
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kIntStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(IntFieldParse_strtoll)->Name("IntFieldParse/strtoll");

/**
 * Convert a single field from string to int using sscanf.
 *
 * sscanf should be avoided because it does not report all conversion errors.
 */
static void IntFieldParse_sscanf(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kIntStrings) {
            int64_t value;
            if (sscanf(field.c_str(), "%lld", &value) != 1) { // NOLINT(cert-err34-c)
                break; // error checking
            }
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kIntStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(IntFieldParse_sscanf)->Name("IntFieldParse/sscanf");

/**
 * Convert a single field from string to int using istringstream.
 */
static void IntFieldParse_istringstream(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kIntStrings) {
            int64_t value;

            std::istringstream iss(field);
            iss >> value;

            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kIntStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(IntFieldParse_istringstream)->Name("IntFieldParse/istringstream");
