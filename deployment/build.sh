#!/bin/bash

set -e

echo "This script fetches and builds the exact versions of opencv needed"
echo "by anakin, compiles them, then compiles anakin against them."
echo "Running this script will put lots of files in your current directory"
echo "If you don't want this, hit ^C now, otherwise enter."
read x

tag=`date +"%Y%m%d"`

if [[ ! "$1" = "" ]]; then
    tag="$1"
fi

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

echo "Checking for tesseract"
if [ ! -e tesseract-ocr ]; then
    git clone https://code.google.com/p/tesseract-ocr/
    pushd tesseract-ocr
    git checkout 15d48361b4f5 # 3.03-rc1 plus fixes
    popd
fi

echo "Building leptonica"
if [ ! -e install/include/leptonica ]; then
  if [ ! -e leptonica-1.71.tar.gz ]; then
     wget http://leptonica.org/source/leptonica-1.71.tar.gz
  fi
  if [ ! -e leptonica-1.71 ]; then
     tar xzf leptonica-1.71.tar.gz
  fi
  pushd leptonica-1.71
  ./configure --prefix=$PWD/../install
  make
  make install
  popd
fi

echo "Building tesseract"
if [ ! -e install/bin/tesseract ]; then
  pushd tesseract-ocr
  export LIBLEPT_HEADERSDIR=$PWD/../install/include
  ./autogen.sh
  ./configure --prefix=$PWD/../install --with-extra-libraries=$PWD/../install/lib
  make
  make install
  popd
fi

echo "Fetching english"
if [ ! -e tesseract-ocr-3.02.eng.tar.gz ]; then
  wget https://tesseract-ocr.googlecode.com/files/tesseract-ocr-3.02.eng.tar.gz
  tar xzf tesseract-ocr-3.02.eng.tar.gz
fi

echo "Building opencv+opencv_contrib"
mkdir -p build_opencv
pushd build_opencv
cmake $cmake_flags -DCMAKE_PREFIX_PATH=$PWD/../install/ \
  -DBUILD_WITH_DEBUG_INFO=OFF \
  -DOPENCV_EXTRA_MODULES_PATH=$PWD/../opencv_contrib/modules \
  -DENABLE_PRECOMPILED_HEADERS=OFF \
  -DBUILD_PERF_TESTS=OFF -DBUILD_TESTS=OFF \
  $PWD/../opencv
make
popd

echo "Checking for anakin based on deployment script location"
SCRIPT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
ANAKIN_DIR=$SCRIPT_DIR/..
echo "Anakin should be in $ANAKIN_DIR"

echo "Building anakin"
mkdir -p build_${tag}
pushd build_${tag}
cmake $cmake_flags -DOpenCV_DIR=$PWD/../build_opencv $ANAKIN_DIR
make
popd

rm -f current
ln -s build_${tag} current
