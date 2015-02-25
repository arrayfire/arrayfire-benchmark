#!/bin/bash

# A bash script to update the ArrayFire benchmarking utility, all required
# non-system libraries, and test data.

ROOT_DIR=~/benchmarking

# Update clBLAS
cd ${ROOT_DIR}/clBLAS
git pull
cd build
cmake ../src -DCMAKE_BUILD_TYPE=Release
make -j8
make install

# Update clFFT
cd ${ROOT_DIR}/clFFT
git pull
cd build
cmake ../src -DCMAKE_BUILD_TYPE=Release
make -j8
make install

# update boost compute
# this is a header-only library, so we need not compile anything
cd ${ROOT_DIR}/compute
git pull

# update ArrayFire
cd ${ROOT_DIR}/arrayfire
git pull
cd build
cmake -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/package ..
make -j8
make install

# update the ArrayFire benchmark utility
cd ${ROOT_DIR}/arrayfire_benchmark
git pull
cd build
cmake ..
make -j8
