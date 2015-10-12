## Description

Benchmark software for [ArrayFire](https://github.com/arrayfire/arrayfire)

## Prerequisites

### System libraries

The benchmarking program requires the following system-level libraries:

* ncurses
* python
* matplotlib

On Ubuntu, you may install these packages using the following command:

    sudo apt-get install libncurses5-dev python-matplotlib

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

## Building on Windows
Install ArrayFire using the installer. Advanced users can opt to use custom
builds, but this document will not detail steps for that.

You will also need Boost. You can install it using the Boost binary installers
for Windows (VS2013 builds).

Open the CMake GUI. Source directory is arrayfire-benchmark and build
directory is arrayfire-benchmark/build. Hit configure.

You may need to add/change the following:
* Add `BOOST_ROOT` and point it to the boost install directory.
* Add `BOOST_LIBRARYDIR` and point it to `BOOST_ROOT`/lib64-msvc-12.0

Run configure again.

Once the Boost libraries are found, you will need to add a prefix of "lib" to
all the boost libraries. Example:
> `BOOST_SYSTEM_LIBRARY_RELEASE` will change from
> `C:/boost/lib64-msvc-12.0/boost_system-vc120-mt-1_58.lib` to
> `C:/boost/lib64-msvc-12.0/libboost_system-vc120-mt-1_58.lib`.

Run configure once again. Then generate.

Now open the `build/af_benchmark` solution and build it.

## Using the benchmark suite

First generate a series of benchmark results by running one of
`benchmark_cpu`, `benchmark_opencl`, or `benchmark_cuda` programs with the
`-r output_file.csv` option. These three programs have the same set of options
which may be seen using the `-h` option.

After this, use the `scripts/plot_results.py` to visualize the results of
the benchmark suite. Please use `python scripts/plot_results.py -h` to see
a full list of options for this tool.
