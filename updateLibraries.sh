#!/bin/bash

# A bash script to update the ArrayFire benchmarking utility, all required
# non-system libraries, and test data.

ROOT_DIR=~/benchmarking

# Ensure we have a copy of ArrayFire on the system
cd ${ROOT_DIR}
if [ ! -d ${ROOT_DIR}/arrayfire ] ; then
    git clone https://github.com/arrayfire/arrayfire.git
fi
# update ArrayFire and all submodules
cd ${ROOT_DIR}/arrayfire
git pull
git submodule init
git submodule update
# Compile ArrayFire and install it to a local directory
mkdir -p ${ROOT_DIR}/arrayfire/build
cd ${ROOT_DIR}/arrayfire/build
# configure ArrayFire to build as quickly as possible
cmake -DCMAKE_INSTALL_PREFIX=${ROOT_DIR}/package \
    -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DBUILD_TEST=OFF \
    -DBUILD_GRAPHICS=OFF ..
make -j8
make install

# Ensure the benchmark suite exists on the local machine
cd ${ROOT_DIR}
if [ ! -d ${ROOT_DIR}/arrayfire_benchmark ] ; then
    git clone https://github.com/bkloppenborg/arrayfire_benchmark
fi
# update the benchmark suite
cd ${ROOT_DIR}/arrayfire_benchmark
git pull
git submodule init
git submodule update
# compile it
cd build
cmake -DArrayFire_DIR=${ROOT_DIR}/package/share/ArrayFire/cmake ..
make -j8
