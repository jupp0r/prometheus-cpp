#!/bin/bash

set -euo pipefail

THIRDPARTY_ROOT=$(cd $(dirname "${BASH_SOURCE[0]}") && /bin/pwd -P)
INSTALL_PREFIX="${TRAVIS_BUILD_DIR:?}/_opt"

mkdir "${THIRDPARTY_ROOT}/civetweb/_build"
cd "${THIRDPARTY_ROOT}/civetweb/_build"
cmake  .. -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" -DCIVETWEB_ENABLE_CXX=ON -DCIVETWEB_ENABLE_SSL=OFF -DBUILD_TESTING=OFF
make -j4
make install
mv "${INSTALL_PREFIX}/lib/libcxx-library.a" "${INSTALL_PREFIX}/lib/libcivetweb-cpp.a"

mkdir "${THIRDPARTY_ROOT}/cpr/_build"
cd "${THIRDPARTY_ROOT}/cpr/_build"
cmake  .. -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" -DBUILD_CPR_TESTS=OFF -DUSE_SYSTEM_CURL=ON
make -j4
cp lib/libcpr.a "${INSTALL_PREFIX}/lib/"
cp -r ../include/cpr "${INSTALL_PREFIX}/include/"

mkdir "${THIRDPARTY_ROOT}/googletest/_build"
cd "${THIRDPARTY_ROOT}/googletest/_build"
cmake  .. -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}"
make -j4
make install
