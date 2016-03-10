#!/bin/bash

# A bash script to update the ArrayFire benchmarking utility, all required
# non-system libraries, and test data.

ROOT_DIR=/tmp/benchmarking
AF_DIR=${ROOT_DIR}/arrayfire-3

build_arrayfire()
{
    # Ensure we have a copy of ArrayFire on the system
    echo ""
    echo "Downloading/updating ArrayFire Source"
    echo ""
    pushd ${ROOT_DIR}
    if [ ! -d ${AF_DIR} ] ; then
        git clone --recursive https://github.com/arrayfire/arrayfire.git arrayfire_source
    fi
    cd ${ROOT_DIR}/arrayfire_source
    git co $1
    git submodule init
    git submodule update
    # Compile ArrayFire and install it to a local directory
    mkdir -p ${ROOT_DIR}/arrayfire_source/build
    cd ${ROOT_DIR}/arrayfire_source/build
    # configure ArrayFire to build as quickly as possible
    #cmake -DCMAKE_INSTALL_PREFIX=${AF_DIR} \
    #    -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF -DBUILD_TEST=OFF \
    #    -DBUILD_GRAPHICS=OFF ..
    #make -j8
    #make install
    popd
}

install_arrayfire()
{
    echo ""
    echo "Downloading/installing ArrayFire Installer"
    echo ""
    VER=$1
    pushd ${ROOT_DIR}
    if [ ! -d ${AF_DIR} ] ; then
        OS=`uname`
        ARCH=`uname -m`
        if [[ "${OS}" == "Darwin" ]]; then
            #wget http://arrayfire.com/installer_archive/${VER}/ArrayFire-no-gl-v${VER}_OSX.pkg
            echo "Cannot download and install OSX Installer from command line"
            exit 1
        elif [[ "${OS}" == "Linux" ]]; then
            if [[ "${ARCH}" == "x86_64" ]]; then
                wget http://arrayfire.com/installer_archive/${VER}/ArrayFire-no-gl-v${VER}_Linux_x86_64.sh
            else
                if [[ "${ARCH}" == "aarch64" ]]; then
                    wget http://arrayfire.com/installer_archive/${VER}/ArrayFire-no-gl-v${VER}_Linux_aarch64.sh
                else
                    wget http://arrayfire.com/installer_archive/${VER}/ArrayFire-no-gl-v${VER}_Linux_armv7l.sh
                fi
            fi
            ## Verify MD5SUM
            #MD5_GOLD=3c781b43a34d2bea9727223e99106e51
            #MD5_CHEK=`md5sum ArrayFire-3.3.0_Linux_x86_64.sh`
            #if [ "${MD5_GOLD}" != ${MD5_CHEK}]; then
            #    echo "MD5 Sums do not match. Exiting..."
            #    exit 1
            #fi
            # Install to ${ROOT_DIR}/arrayfire-3
            sh ArrayFire-no-gl-v${VER}_Linux_${ARCH}.sh --include-subdir --skip-licence
        fi
    fi
    popd
}

help_func()
{
    echo "Options:"
    echo "--prefix=<dir>: Path to working directory. /tmp/benchmarking is default."
    echo "--build: Download and build ArrayFire from source"
    echo "--install: Path to working directory (default)"
    echo "--release=<commit hash/branch/tag/installer version>: Version of ArrayFire to build or install.
    Hash/Branch/Tag will work for --build (default is master).
    3 Decimal release ID (eg. 3.1.0) will work for --install. (default is 3.3.0)"
    echo "--af-dir=<dir>: Where ArrayFire is installed/built. Contents of this directory must be include, lib, share/ArrayFire/cmake"
    exit 0
}

build_source=""
release=""

for a in "$@"; do
    if echo $a | grep "^--prefix=" > /dev/null 2> /dev/null; then
        ROOT_DIR=`echo $a | sed "s/^--prefix=//"`
        AF_DIR=${ROOT_DIR}/arrayfire-3
    fi
    if echo $a | grep "^--help" > /dev/null 2> /dev/null; then
        help_func
    fi
    if echo $a | grep "^--build" > /dev/null 2> /dev/null; then
        build_source=true
    fi
    if echo $a | grep "^--install" > /dev/null 2> /dev/null; then
        build_source=false
    fi
    if echo $a | grep "^--release=" > /dev/null 2> /dev/null; then
        release=`echo $a | sed "s/^--release=//"`
    fi
    if echo $a | grep "^--af-dir=" > /dev/null 2> /dev/null; then
        AF_DIR=`echo $a | sed "s/^--af-dir=//"`
    fi
done

mkdir -p ${ROOT_DIR}

# By default install arrayfire
if [ ! -d ${AF_DIR} ] ; then
    if [ "x${build_source}x" == "xtruex" ]; then
        if [ -z "${release}" ]; then
            build_arrayfire "master"
        else
            build_arrayfire ${release}
        fi
    else
        if [ -z "${release}" ]; then
            install_arrayfire "3.3.0"
        else
            install_arrayfire ${release}
        fi
    fi
fi

# Ensure the benchmark suite exists on the local machine
echo ""
echo "Downloading/updating the benchmark suite."
echo ""
pushd ${ROOT_DIR}
if [ ! -d ${ROOT_DIR}/arrayfire-benchmark ] ; then
    git clone --recursive https://github.com/bkloppenborg/arrayfire-benchmark
fi
# update the benchmark suite
cd ${ROOT_DIR}/arrayfire-benchmark
# compile it
cd build
rm -rf ./*

echo cmake -DArrayFire_DIR=${AF_DIR}/share/ArrayFire/cmake ..
cmake -DArrayFire_DIR=${AF_DIR}/share/ArrayFire/cmake ..
make -j8
popd
