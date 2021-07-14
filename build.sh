#! /bin/bash

# sudo apt update && sudo apt install -y zlib1g-dev curl libcurl4-openssl-dev

# fetch third-party dependencies
git submodule init
git submodule update

mkdir -p build
cd build || exit

# run cmake
cmake .. -DBUILD_SHARED_LIBS=OFF # or OFF for static libraries

# build
make -j 4

# # run tests
# ctest -V

# install the libraries and headers
mkdir -p deploy

make DESTDIR=`pwd`/deploy install

# For local build of other application
sudo make install