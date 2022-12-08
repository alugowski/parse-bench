Benchmark different approaches to parsing scientific datafiles.

## Parse Bench

C++ IO Streams are often the go-to implementation choice. While easy to use and very featureful, iostream throughput is terrible.

Benchmarks in this repo measure throughput of several approaches for each of several parsing steps to show the performance impact of each approach:
 * **Scan**: a simple iteration reading each byte. Shows the roofline performance supported by the memory subsystem.
 * **Newline detection**: A scan that finds newlines. Show the performance hit of testing characters.
 * **Line tokenization**: A scan that also splits each line into fields based on whitespace.
 * **Parse integers**: Test approaches to parsing integers.
 * **Parse double**: Test approaches to parsing floating point numbers.
 * **Parse block**: Use lessons learned to write a parser that can parse a simple datafile.

The datafile lines we test contain two integers and a double. Of course other schemas will show different performance, but this provides good ballpark figures.

Parsing methods do not attempt to cheat by skipping error checking (where possible) as a real parser will have to perform error checking.

## Results

The benchmarks are run using Google Benchmark. The default settings emit a table that shows throughput in bytes per second.

The core result is bytes per second for each approach. That is the only figure shared by all the different steps, and the most useful figure to estimate how long a parser will take to parse a file.

We use short and long fields to attempt to not cherry-pick easy or hard inputs. See [main.cpp](main.cpp) for what the input data looks like.

The [plots](plots) directory contains a Jupyter notebook that can read the Google Benchmark output and plot the results.

## Takeaways

* Datafile parse performance is CPU bound due to parsing, not IO bound. By a lot.
* IO Streams are very slow, about 10x slower than a mechanical hard drive.
* C routines like scanf are faster but still slow.
* A fast approach includes using fast methods to identify fields then use fast field conversion methods to parse said fields.