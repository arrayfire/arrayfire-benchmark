## Description

Benchmark software for [ArrayFire](https://github.com/arrayfire/arrayfire)

## Prerequisites

[ArrayFire](https://github.com/arrayfire/arrayfire)

## Checkout and build

Basic building instructions:

    git clone https://github.com/bkloppenborg/arrayfire_benchmark.git
    cd arrayfire_benchmark
    git submodule update --init
    cd build
    cmake ..
    make

If you have ArrayFire installed in a non-standard location, specify
`-DArrayFire_ROOT_DIR=/path/to/arrayfire` as part of the `cmake` command.

## Using the benchmark suite

First generate a series of benchmark results by running one of 
`benchmark_cpu`, `benchmark_opencl`, or `benchmark_cuda` programs with the
`-r output_file.csv` option. These three programs have the same set of options
which may be seen using the `-h` option.

After this, use the `scripts/plot_results.py` to visualize the results of
the benchmark suite. Please use `python scripts/plot_results.py -h` to see
a full list of options for this tool.
