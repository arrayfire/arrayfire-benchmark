#!/bin/bash

# A bash script to update the ArrayFire benchmarking utility, all required
# non-system libraries, and test data.

ROOT_DIR=~/benchmarking

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
