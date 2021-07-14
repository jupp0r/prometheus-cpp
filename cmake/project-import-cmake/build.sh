#! /bin/bash

rm -rf build
mkdir -p build
cd build || exit
cmake ..
make