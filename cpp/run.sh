#!/bin/bash

# Set the compiler, if needed

if [ -z "$CXX" ] ; then
	# no compiler selected, try to find one

	if command -v brew &> /dev/null ; then
		# Homebrew
		# This is useful since the version of clang provided with macOS does not include OpenMP
		if brew --prefix llvm &> /dev/null ; then
			export CC_="$(brew --prefix llvm)/bin/clang"
			export CXX_="$(brew --prefix llvm)/bin/clang++"
		elif brew --prefix gcc &> /dev/null ; then
			export CC_="$(brew --prefix gcc)/bin/gcc"
			export CXX_="$(brew --prefix gcc)/bin/g++"
		fi
		if [ -f "$CXX_" ]; then
			export CC="$CC_"
			export CXX="$CXX_"
		fi
	fi
fi

# Build
cmake -S . -B cmake-build-release/ -D CMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release/ --target parse-bench

# run
cmake-build-release/parse-bench --benchmark_out_format=json --benchmark_out=../plots/benchmark_outputs/C++.json