if [%1] == [bazel] (
    choco install bazel -y || EXIT /B 1
)

if [%1] == [cmake] (
    %VCPKG_INSTALLATION_ROOT%/vcpkg install benchmark civetweb curl gtest zlib || EXIT /B 1
)

