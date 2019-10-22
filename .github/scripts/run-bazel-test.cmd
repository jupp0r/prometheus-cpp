bazel build //... || EXIT /B 1

bazel test --test_output=all //core/... //pull/... || EXIT /B 1

bazel run -c opt //core/benchmarks || EXIT /B 1
