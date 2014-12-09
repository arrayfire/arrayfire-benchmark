## Description

Benchmark software for [ArrayFire](https://github.com/arrayfire/arrayfire)

## Prerequisites

[ArrayFire](https://github.com/arrayfire/arrayfire)

## Checkout and build

Basic building instructions:

```
git clone https://github.com/bkloppenborg/arrayfire_benchmark.git
cd arrayfire_benchmark
git submodule update --init
cd build
cmake ..
make
```

If you have ArrayFire installed in a non-standard location, specify
`-DArrayFire_ROOT_DIR=/path/to/arrayfire` as part of the `cmake` command.
