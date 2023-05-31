/*
Copyright (C) 2019-2023 Adam Lugowski

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "parse_bench.h"

#include <cstdio>
#include <cerrno>
#include <sstream>
#include <string>
#include <charconv>

#include "double-conversion/double-conversion.h"
#include <boost/charconv.hpp>
#include <dragonbox/dragonbox_to_chars.h>
#include <ryu/ryu.h>

/**
 * Convert a single field from double to string using std::to_chars.
 */
static void DoubleFieldFormat_to_chars_shortest(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    std::string field(1024, ' ');

    errno = 0;
    for ([[maybe_unused]] auto _ : state) {
        for (const auto& d : kDoubles) {
            std::to_chars_result result = std::to_chars(field.data(), field.data() + field.size(), d);

            if (result.ec != std::errc()) {
                break; // error testing
            }

            auto length = result.ptr - field.data();

            benchmark::DoNotOptimize(field);
            num_bytes += length;
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldFormat_to_chars_shortest)->Name("DoubleFieldFormat/std::to_chars(shortest)" COMPILER);

/**
 * Convert a single field from double string using the Google double-conversion library.
 */
static void DoubleFieldFormat_double_conversion_ToShortest(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    double_conversion::DoubleToStringConverter converter(double_conversion::DoubleToStringConverter::NO_FLAGS,
                                                         "inf", "nan", 'e', -4, 6, 0, 0);

    const int kBufferSize = 128;
    char buffer[kBufferSize];
    double_conversion::StringBuilder builder(buffer, kBufferSize);


    for ([[maybe_unused]] auto _ : state) {
        for (const auto& d : kDoubles) {
            builder.Reset();
            if (!converter.ToShortest(d, &builder)) {
                break; // error testing
            }
            std::string field = builder.Finalize();
            benchmark::DoNotOptimize(field);
            num_bytes += field.size();
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldFormat_double_conversion_ToShortest)->Name("DoubleFieldFormat/double-conversion(ToShortest)");

/**
 * Convert a single field from double to string using CppAlliance's candidate for Boost.charconv
 */
static void DoubleFieldFormat_CppAlCharconv(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    std::string field(1024, ' ');

    errno = 0;
    for ([[maybe_unused]] auto _ : state) {
        for (const auto& d : kDoubles) {
            auto result = boost::charconv::to_chars(field.data(), field.data() + field.size(), d);

            if (result.ec != std::errc()) {
                break; // error testing
            }

            auto length = result.ptr - field.data();

            benchmark::DoNotOptimize(field);
            num_bytes += length;
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldFormat_CppAlCharconv)->Name("DoubleFieldFormat/CppAlliance-charconv(shortest)");

/**
 * Convert a single field from double to string using Dragonbox.
 */
static void DoubleFieldFormat_Dragonbox(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    std::string field(1024, ' ');

    errno = 0;
    for ([[maybe_unused]] auto _ : state) {
        for (const auto& d : kDoubles) {
            char *end_ptr = jkj::dragonbox::to_chars(d, field.data());

            auto length = end_ptr - field.data();

            benchmark::DoNotOptimize(field);
            num_bytes += length;
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldFormat_Dragonbox)->Name("DoubleFieldFormat/Dragonbox");

/**
 * Convert a single field from double to string using Ryu (shortest representation).
 */
static void DoubleFieldFormat_Ryu_shortest(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    std::string field(1024, ' ');

    errno = 0;
    for ([[maybe_unused]] auto _ : state) {
        for (const auto& d : kDoubles) {
            auto length = d2s_buffered_n(d, field.data());

            benchmark::DoNotOptimize(field);
            num_bytes += length;
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldFormat_Ryu_shortest)->Name("DoubleFieldFormat/Ryu(shortest)");

/**
 * Convert a single field from double to string.
 */
static void DoubleFieldFormat_to_string(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    errno = 0;
    for ([[maybe_unused]] auto _ : state) {
        for (const auto& d : kDoubles) {
            std::string field = std::to_string(d);
            benchmark::DoNotOptimize(field);
            num_bytes += field.size();
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldFormat_to_string)->Name("DoubleFieldFormat/std::to_string");

/**
 * Convert a single field from double to string using sscanf.
 *
 * sscanf should be avoided because it does not report all conversion errors.
 */
static void DoubleFieldFormat_snprintf(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& d : kDoubles) {
            std::string field(1024, ' ');
            auto length = snprintf(field.data(), field.size(), "%lf", d);
            if (length == 0) {
                break; // error checking
            }
            benchmark::DoNotOptimize(field);
            num_bytes += length;
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldFormat_snprintf)->Name("DoubleFieldFormat/snprintf");

/**
 * Convert a single field from double to string using ostringstream.
 */
static void DoubleFieldFormat_ostringstream(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& d : kDoubles) {
            std::string field;

            std::ostringstream oss;
            oss << d;
            field = oss.str();

            benchmark::DoNotOptimize(field);
            num_bytes += field.size();
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldFormat_ostringstream)->Name("DoubleFieldFormat/ostringstream");
