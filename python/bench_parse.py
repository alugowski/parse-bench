# Copyright (C) 2022-2023 Adam Lugowski
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
# the following disclaimer in the documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# See Python Google Benchmark examples:
# https://github.com/google/benchmark/blob/main/bindings/python/google_benchmark/example.py

from io import StringIO
import google_benchmark as benchmark
from google_benchmark import Counter

import numpy as np

# same strings as in the C++ benchmark (see main.cpp)
int_str = ["123456", "1"]
floats = [123456., 1., 333.323]
float_str = ["123456", "1", "333.323"]
lines = [
    "123456 234567 333.323",
    "1 234567 333.323",
    "1 2 3",
]

int_str_list = int_str
while len(int_str_list) < 1000:
    int_str_list += int_str_list

float_list = floats
while len(float_list) < 1000:
    float_list += float_list

float_str_list = float_str
while len(float_str_list) < 1000:
    float_str_list += float_str_list


def create_block():
    str_list = lines
    while len(str_list) < 100_000:
        str_list += str_list
    return "\n".join(str_list)


block = create_block()


@benchmark.register(name="IntFieldParse/[int()]")
def parse_int_list_comprehension(state):
    while state:
        _ = [int(element) for element in int_str_list]
    state.bytes_processed = state.iterations * sum([len(element) for element in int_str_list])
    state.counters["fields_converted_per_second"] = Counter(state.iterations * len(int_str_list), Counter.kIsRate)


@benchmark.register(name="IntFieldParse/np.loadtxt")
def parse_int_numpy_loadtxt(state):
    while state:
        _ = np.loadtxt(int_str_list, dtype=np.int64)
    state.bytes_processed = state.iterations * sum([len(element) for element in int_str_list])
    state.counters["fields_converted_per_second"] = Counter(state.iterations * len(int_str_list), Counter.kIsRate)


@benchmark.register(name="DoubleFieldParse/[float()]")
def parse_float_list_comprehension(state):
    while state:
        _ = [float(element) for element in float_str_list]
    state.bytes_processed = state.iterations * sum([len(element) for element in float_str_list])
    state.counters["fields_converted_per_second"] = Counter(state.iterations * len(float_str_list), Counter.kIsRate)


@benchmark.register(name="DoubleFieldFormat/[str()]")
def parse_float_list_comprehension(state):
    while state:
        _ = [float(element) for element in float_list]
    state.bytes_processed = state.iterations * sum([len(element) for element in float_str_list])
    state.counters["fields_converted_per_second"] = Counter(state.iterations * len(float_str_list), Counter.kIsRate)


@benchmark.register(name="DoubleFieldParse/np.loadtxt")
def parse_float_numpy_loadtxt(state):
    while state:
        _ = np.loadtxt(float_str_list, dtype=np.float64)
    state.bytes_processed = state.iterations * sum([len(element) for element in float_str_list])
    state.counters["fields_converted_per_second"] = Counter(state.iterations * len(float_str_list), Counter.kIsRate)


@benchmark.register(name="SplitLines/readlines")
def parse_splitlines_lines(state):
    while state:
        for _ in StringIO(block).readlines():
            pass
    state.bytes_processed = state.iterations * len(block)


@benchmark.register(name="BlockParse/np.genfromtxt")
def parse_block_numpy_genfromtxt(state):
    while state:
        _ = np.genfromtxt(StringIO(block), dtype=(np.int64, np.int64, np.float64))
    state.bytes_processed = state.iterations * len(block)


@benchmark.register(name="BlockParse/np.loadtxt")
def parse_block_numpy_loadtxt(state):
    while state:
        _ = np.loadtxt(StringIO(block), dtype=np.float64)
    state.bytes_processed = state.iterations * len(block)


if __name__ == "__main__":
    benchmark.main()
