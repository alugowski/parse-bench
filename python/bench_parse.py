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

import google_benchmark as benchmark
from google_benchmark import Counter

# same strings as in the C++ benchmark (see main.cpp)
ints = ["123456", "1"]
floats = ["123456", "1", "333.323"]


@benchmark.register(name="IntFieldParse/int()")
def parse_int(state):
    while state:
        # for i_str in ints:
        #     _ = int(i_str)
        # unroll the loop to avoid timing loop code
        _ = int("123456")
        _ = int("1")
    state.bytes_processed = state.iterations * sum([len(element) for element in ["123456", "1"]])
    state.counters["fields_converted_per_second"] = Counter(state.iterations*2, Counter.kIsRate)


@benchmark.register(name="IntFieldParse/[int()]")
def parse_int_list_comprehension(state):
    str_list = ints
    while len(str_list) < 1000:
        str_list += str_list

    list_elements = len(str_list)
    list_bytes = sum([len(element) for element in str_list])

    while state:
        _ = [int(element) for element in str_list]
    state.bytes_processed = state.iterations * list_bytes
    state.counters["fields_converted_per_second"] = Counter(state.iterations*list_elements, Counter.kIsRate)


@benchmark.register(name="DoubleFieldParse/float()")
def parse_float(state):
    while state:
        # for f_str in floats:
        #     _ = float(f_str)
        # unroll the loop to avoid timing loop code
        _ = float("123456")
        _ = float("1")
        _ = float("333.323")

    state.bytes_processed = state.iterations * sum([len(element) for element in ["123456", "1", "333.323"]])
    state.counters["fields_converted_per_second"] = Counter(state.iterations*3, Counter.kIsRate)


@benchmark.register(name="IntFieldParse/[float()]")
def parse_float_list_comprehension(state):
    str_list = floats
    while len(str_list) < 1000:
        str_list += str_list

    list_elements = len(str_list)
    list_bytes = sum([len(element) for element in str_list])

    while state:
        _ = [float(element) for element in str_list]
    state.bytes_processed = state.iterations * list_bytes
    state.counters["fields_converted_per_second"] = Counter(state.iterations*list_elements, Counter.kIsRate)


if __name__ == "__main__":
    benchmark.main()
