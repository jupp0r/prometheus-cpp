# Prometheus Client Library for Modern C++ [![Build Status](https://travis-ci.org/jupp0r/prometheus-cpp.svg?branch=master)](https://travis-ci.org/jupp0r/prometheus-cpp)[![Coverity Scan](https://scan.coverity.com/projects/10567/badge.svg)](https://scan.coverity.com/projects/jupp0r-prometheus-cpp)

This library aims to enable
[Metrics-Driven Development](https://sookocheff.com/post/mdd/mdd/) for
C++ services. It implements the
[Prometheus Data Model](https://prometheus.io/docs/concepts/data_model/),
a powerful abstraction on which to collect and expose metrics. We
offer the possibility for metrics to be collected by Prometheus, but
other push/pull collections can be added as plugins.

## Usage

``` c++
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include <prometheus/exposer.h>
#include <prometheus/registry.h>

int main(int argc, char** argv) {
  using namespace prometheus;

  // create an http server running on port 8080
  Exposer exposer{"127.0.0.1:8080"};

  // create a metrics registry with component=main labels applied to all its
  // metrics
  auto registry = std::make_shared<Registry>();

  // add a new counter family to the registry (families combine values with the
  // same name, but distinct label dimensions)
  auto& counter_family = BuildCounter()
                             .Name("time_running_seconds")
                             .Help("How many seconds is this server running?")
                             .Labels({{"label", "value"}})
                             .Register(*registry);

  // add a counter to the metric family
  auto& second_counter = counter_family.Add(
      {{"another_label", "value"}, {"yet_another_label", "value"}});

  // ask the exposer to scrape the registry on incoming scrapes
  exposer.RegisterCollectable(registry);

  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // increment the counter by one (second)
    second_counter.Increment();
  }
  return 0;
}
```

## Requirements

Using `prometheus-cpp` requires a C++11 compliant compiler. It has been successfully tested with GNU GCC 4.8 on Ubuntu Trusty and Visual Studio 2017 (but Visual Studio 2015 should work, too).

## Building

There are two supported ways to build
`prometheus-cpp` - [CMake](https://cmake.org)
and [bazel](https://bazel.io). Both are tested in CI and should work
on master and for all releases.

In case these instructions don't work for you, looking at
the [travis build script](.travis.yml) might help.

### via CMake

For CMake builds don't forget to fetch the submodules first. Then build as usual.

``` shell
# fetch third-party dependencies
git submodule init
git submodule update

mkdir _build
cd _build

# run cmake
cmake ..

# build
make -j 4

# run tests
ctest -V

# install the libraries and headers
mkdir -p deploy
make DESTDIR=`pwd`/deploy install
```

### via Bazel

Install [bazel](https://www.bazel.io).  Bazel makes it easy to add
this repo to your project as a dependency. Just add the following
to your `WORKSPACE`:

```python
http_archive(
    name = "com_github_jupp0r_prometheus_cpp",
    strip_prefix = "prometheus-cpp-master",
    urls = ["https://github.com/jupp0r/prometheus-cpp/archive/master.zip"],
)

load("@com_github_jupp0r_prometheus_cpp//:repositories.bzl", "prometheus_cpp_repositories")

prometheus_cpp_repositories()
```

Then, you can reference this library in your own `BUILD` file, as
demonstrated with the sample server included in this repository:

```python
cc_binary(
    name = "sample_server",
    srcs = ["sample_server.cc"],
    deps = ["@com_github_jupp0r_prometheus_cpp//:prometheus_cpp"],
)
```

When you call `prometheus_cpp_repositories()` in your `WORKSPACE` file,
you introduce the following dependencies, if they do not exist yet, to your project:

* `load_civetweb()` to load `civetweb` rules for Civetweb
* `load_com_google_googletest()` to load `com_google_googletest` rules for Google gtest
* `load_com_google_googlebenchmark()` to load `com_github_google_benchmark` rules for Googlebenchmark
* `load_com_github_curl()` to load `com_github_curl` rules for curl
* `load_net_zlib_zlib()` to load `net_zlib_zlib` rules for zlib

The list of dependencies is also available from file `repositories.bzl`.


## Contributing

Please adhere to the [Google C++ Style
Guide](https://google.github.io/styleguide/cppguide.html). Make sure
to clang-format your patches before opening a PR. Also make sure to
adhere to [these commit message
guidelines](https://chris.beams.io/posts/git-commit/).

You can check out this repo and build the library using
``` bash
bazel build //...           # build everything
bazel build //core //pull   # build just the libraries
```

Run the unit tests using
```
bazel test //...
```

There is also an integration test that
uses [telegraf](https://github.com/influxdata/telegraf) to scrape a
sample server. With telegraf installed, it can be run using
```
bazel test //pull/tests/integration:scrape-test
```

## Benchmarks

There's a benchmark suite you can run:

```
bazel run -c opt //core/tests/benchmark

INFO: Found 1 target...
Target //core/tests/benchmark:benchmark up-to-date:
  bazel-bin/core/tests/benchmark/benchmark
INFO: Elapsed time: 1.682s, Critical Path: 1.56s

INFO: Running command line: bazel-bin/core/tests/benchmark/benchmark
Run on (8 X 2300 MHz CPU s)
2016-10-17 15:56:49
Benchmark                              Time           CPU Iterations
--------------------------------------------------------------------
BM_Counter_Increment                  11 ns         11 ns   62947942
BM_Counter_Collect                    84 ns         84 ns    8221752
BM_Gauge_Increment                    11 ns         11 ns   61384663
BM_Gauge_Decrement                    11 ns         11 ns   62148197
BM_Gauge_SetToCurrentTime            199 ns        198 ns    3589670
BM_Gauge_Collect                      86 ns         85 ns    7469136
BM_Histogram_Observe/0               122 ns        122 ns    5839855
BM_Histogram_Observe/1               116 ns        115 ns    5806623
BM_Histogram_Observe/8               126 ns        126 ns    5781588
BM_Histogram_Observe/64              138 ns        138 ns    4895550
BM_Histogram_Observe/512             228 ns        228 ns    2992898
BM_Histogram_Observe/4k              959 ns        958 ns     642231
BM_Histogram_Collect/0               328 ns        327 ns    2002792
BM_Histogram_Collect/1               356 ns        354 ns    1819032
BM_Histogram_Collect/8              1553 ns       1544 ns     454921
BM_Histogram_Collect/64            10389 ns      10287 ns      66759
BM_Histogram_Collect/512           75795 ns      75093 ns       9075
BM_Histogram_Collect/4k           615853 ns     610277 ns       1222
BM_Registry_CreateFamily             195 ns        182 ns    3843894
BM_Registry_CreateCounter/0          319 ns        317 ns    1914132
BM_Registry_CreateCounter/1         2146 ns       2131 ns     408432
BM_Registry_CreateCounter/8         8936 ns       8837 ns      82439
BM_Registry_CreateCounter/64       72589 ns      72010 ns       9248
BM_Registry_CreateCounter/512     694323 ns     686655 ns       1056
BM_Registry_CreateCounter/4k    18246638 ns   18150525 ns         40
```

## Project Status
Beta, getting ready for 1.0. The library is pretty stable and used in
production. There are some small breaking API changes that might
happen before 1.0 Parts of the library are instrumented by itself
(bytes scraped, number of scrapes, scrape request latencies).  There
is a working [example](pull/tests/integration/sample_server.cc) that's
scraped by telegraf as part of integration tests.

## FAQ

### What scrape formats do you support

Only the [Prometheus Text Exposition
Format](https://github.com/prometheus/docs/blob/master/content/docs/instrumenting/exposition_formats.md#text-format-details).
Support for the protobuf format was removed because it's been removed
from Prometheus 2.0.

## License

MIT
