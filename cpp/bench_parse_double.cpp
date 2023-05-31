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
#include <limits>
#include <sstream>
#include <charconv>

#include "fast_float/fast_float.h"
#include "double-conversion/double-conversion.h"
#include <boost/charconv.hpp>

#if defined(__cplusplus) && __cplusplus >= 201703L
#ifdef FROM_CHARS_DOUBLE_SUPPORTED
/**
 * Convert a single field from string to double.
 *
 * Compiler support for a double version of from_chars appears to be very spotty at this time.
 */
static void DoubleFieldParse_from_chars(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kDoubleStrings) {
            double value = 0;
            std::from_chars_result result = std::from_chars(field.data(), field.data() + field.size(), value, std::chars_format::general);
            if (result.ec != std::errc()) {
                break; // error testing
            }
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldParse_from_chars)->Name("DoubleFieldParse/from_chars" COMPILER);
#endif
#endif

/**
 * Convert a single field from string to double using the fast_float library.
 */
static void DoubleFieldParse_from_chars_ff(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kDoubleStrings) {
            double value = 0;
            fast_float::from_chars_result result = fast_float::from_chars(field.data(), field.data() + field.size(), value, fast_float::chars_format::general);
            if (result.ec != std::errc()) {
                break; // error testing
            }
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldParse_from_chars_ff)->Name("DoubleFieldParse/fast_float::from_chars");


/**
 * Convert a single field from string to double using the Google double-conversion library.
 */
static void DoubleFieldParse_double_conversion(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    double_conversion::StringToDoubleConverter converter(double_conversion::StringToDoubleConverter::ALLOW_CASE_INSENSITIVITY, 0, std::numeric_limits<double>::quiet_NaN(), "inf", "nan");

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kDoubleStrings) {
            double value = 0;
            int processed_char_count;
            value = converter.StringToDouble(field.data(), (int)field.size(), &processed_char_count);
            if (processed_char_count == 0) {
                break; // error testing
            }
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldParse_double_conversion)->Name("DoubleFieldParse/double-conversion");

/**
 * Convert a single field from string to double using the CppAlliance charconv library.
 */
static void DoubleFieldParse_CppAlCharconv(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    double_conversion::StringToDoubleConverter converter(double_conversion::StringToDoubleConverter::ALLOW_CASE_INSENSITIVITY, 0, std::numeric_limits<double>::quiet_NaN(), "inf", "nan");

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kDoubleStrings) {
            double value = 0;
            boost::charconv::from_chars_result result = boost::charconv::from_chars(field.data(), field.data() + field.size(), value);
            if (result.ec != std::errc()) {
                break; // error testing
            }
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldParse_CppAlCharconv)->Name("DoubleFieldParse/CppAlliance-charconv");


/**
 * Convert a single field from string to double.
 */
static void DoubleFieldParse_strtod(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    errno = 0;
    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kDoubleStrings) {
            double value = std::strtod(field.c_str(), nullptr);
            if (errno != 0) {
                break; // error checking
            }
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldParse_strtod)->Name("DoubleFieldParse/strtod" COMPILER);

/**
 * Convert a single field from string to double using sscanf.
 *
 * sscanf should be avoided because it does not report all conversion errors.
 */
static void DoubleFieldParse_sscanf(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kDoubleStrings) {
            double value;
            if (sscanf(field.c_str(), "%lf", &value) != 1) { // NOLINT(cert-err34-c)
                break; // error checking
            }
            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldParse_sscanf)->Name("DoubleFieldParse/sscanf");

/**
 * Convert a single field from string to double using istringstream.
 */
static void DoubleFieldParse_istringstream(benchmark::State& state) {
    std::size_t num_bytes = 0;
    std::size_t num_fields = 0;

    for ([[maybe_unused]] auto _ : state) {
        for (const auto& field : kDoubleStrings) {
            double value;

            std::istringstream iss(field);
            iss >> value;

            benchmark::DoNotOptimize(value);
            num_bytes += field.size();
        }
        num_fields += kDoubleStrings.size();
    }

    state.SetBytesProcessed((int64_t)num_bytes);
    state.counters["fields_converted_per_second"] = benchmark::Counter((double)num_fields, benchmark::Counter::kIsRate);
}

BENCHMARK(DoubleFieldParse_istringstream)->Name("DoubleFieldParse/istringstream");
