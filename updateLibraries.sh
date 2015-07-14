#!/bin/bash

# A bash script to update the ArrayFire benchmarking utility, all required
# non-system libraries, and test data.

ROOT_DIR=~/benchmarking

# Ensure we have a copy of ArrayFire on the system
if [ ! -d ${ROOT_DIR}/arrayfire ] ; then
    git checkout https://github.com/arrayfire/arrayfire.git
else
    cd ${ROOT_DIR}/arrayfire
    git pull
fi
# update ArrayFire and all submodules
git submodule init
git submodule update
# Compile ArrayFire and install it to a local directory
mkdir -p ${ROOT_DIR}/arrayfire/build
cd ${ROOT_DIR}/arrayfire/build
cmake -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/package ..
make -j8
make install

# Ensure the benchmark suite exists on the local machine
if [ ! -d ${ROOT_DIR}/arrayfire_benchmark ] ; then
    git checkout https://github.com/bkloppenborg/arrayfire_benchmark
else
    cd ${ROOT_DIR}/arrayfire_benchmark
    git pull
fi
# update the benchmark suite
git submodule init
git submodule update
# compile it
cd build
cmake -DArrayFire_DIR=${ROOT_DIR}/package/arrayfire/share/ArrayFire/cmake ..
make -j8
