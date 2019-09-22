if [%1] == [bazel] (
    choco install bazel -y || EXIT /B 1
)
