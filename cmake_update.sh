#! /bin/bash

# Upgrade CMake
sudo apt remove cmake
wget -nv https://github.com/Kitware/Cmake/releases/download/v3.14.5/cmake-3.14.5.tar.gz
tar -zxvf cmake-3.14.5.tar.gz
cd cmake-3.14.5
./bootstrap
make
sudo make install
cmake --version