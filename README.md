## Description

Benchmark software for [ArrayFire](https://github.com/arrayfire/arrayfire)

## Prerequisites

### System libraries

The benchmarking program requires the following system-level libraries:

* ncurses
* python
* matplotlib

On Ubuntu, you may install these packages using the following command:

    sudo apt-get install libncurses-dev python-matplotlib

### ArrayFire library

Build and install the ArrayFire library following instructions here:

* [ArrayFire](https://github.com/arrayfire/arrayfire)

Note, you may install ArrayFire to a non-system path if needed.

## Checkout and build

Basic building instructions:

    git clone --recursive https://github.com/bkloppenborg/arrayfire_benchmark.git
    cd arrayfire_benchmark
    cd build
    cmake ..
    make

If you have ArrayFire installed in a non-standard location, specify the directory
which contains the `ArrayFireConfig*` files. These files may be found in the
`share/ArrayFire` subdirectory of the installation folder. For example, if ArrayFire 
were installed locally to `/opt/ArrayFire` then we would modify the `cmake` command 
above to be:

```
cmake -DArrayFire_DIR=/opt/ArrayFire/share/ArrayFire ..
```

## Using the benchmark suite

First generate a series of benchmark results by running one of 
`benchmark_cpu`, `benchmark_opencl`, or `benchmark_cuda` programs with the
`-r output_file.csv` option. These three programs have the same set of options
which may be seen using the `-h` option.

After this, use the `scripts/plot_results.py` to visualize the results of
the benchmark suite. Please use `python scripts/plot_results.py -h` to see
a full list of options for this tool.
