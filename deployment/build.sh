#!/bin/bash

set -e

echo "This script fetches and builds the exact versions of opencv needed"
echo "by anakin, compiles them, then compiles anakin against them."
echo "Running this script will put lots of files in your current directory"
echo "If you don't want this, hit ^C now, otherwise enter."
read x

cmake_flags="-DCMAKE_BUILD_TYPE=Release"

echo "Checking for opencv"
if [ ! -e opencv ]; then
    git clone --depth 1 https://github.com/Itseez/opencv.git -b 3.0.0-alpha
fi

echo "Checking for opencv_contrib"
if [ ! -e opencv_contrib ]; then
    git clone https://github.com/Itseez/opencv_contrib.git
    pushd opencv_contrib
    git checkout 0898fb9
    popd
fi

echo "Building opencv+opencv_contrib"
mkdir -p build_opencv
pushd build_opencv
cmake $cmake_flags -DBUILD_WITH_DEBUG_INFO=OFF -DOPENCV_EXTRA_MODULES_PATH=$PWD/../opencv_contrib/modules -DENABLE_PRECOMPILED_HEADERS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_TESTS=OFF $PWD/../opencv
make
popd

echo "Checking for anakin based on deployment script location"
SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
ANAKIN_DIR=$SCRIPT_DIR/..
echo "Anakin should be in $ANAKIN_DIR"

echo "Building anakin"
mkdir -p build
pushd build
cmake $cmake_flags -DOpenCV_DIR=$PWD/../build_opencv $ANAKIN_DIR
make
popd
