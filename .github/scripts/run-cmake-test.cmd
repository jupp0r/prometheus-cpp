mkdir "_build_internal_deps" || EXIT /B 1
cd "_build_internal_deps" || EXIT /B 1
cmake .. -DUSE_THIRDPARTY_LIBRARIES=ON -DENABLE_WARNINGS_AS_ERRORS=ON -DENABLE_COMPRESSION=OFF -DENABLE_PUSH=OFF || EXIT /B 1
cmake --build . --config Debug || EXIT /B 1
cmake --build . --config Release || EXIT /B 1
ctest -C Debug -V -LE Benchmark || EXIT /B 1
ctest -C Release -V || EXIT /B 1
cd .. || EXIT /B 1

mkdir "_build" || EXIT /B 1
cd "_build" || EXIT /B 1
cmake .. -DUSE_THIRDPARTY_LIBRARIES=OFF -DCMAKE_TOOLCHAIN_FILE=%VCPKG_INSTALLATION_ROOT%\scripts\buildsystems\vcpkg.cmake || EXIT /B 1
cmake --build . --config Debug || EXIT /B 1
cmake --build . --config Release || EXIT /B 1
ctest -C Debug -V -LE Benchmark || EXIT /B 1
ctest -C Release -V || EXIT /B 1
cd .. || EXIT /B 1
