# Prometheus Client Library for Modern C++ [![Build Status](https://travis-ci.org/jupp0r/prometheus-cpp.svg?branch=master)](https://travis-ci.org/jupp0r/prometheus-cpp)[![Coverity Scan](https://scan.coverity.com/projects/10567/badge.svg)](https://scan.coverity.com/projects/jupp0r-prometheus-cpp)

This library aims to enable
[Metrics-Driven Development](https://sookocheff.com/post/mdd/mdd/) for
C++ serivices. It implements the
[Prometheus Data Model](https://prometheus.io/docs/concepts/data_model/),
a powerful abstraction on which to collect and expose metrics. We
offer the possibility for metrics to collected by Prometheus, but
other push/pull collections can be added as plugins.

## Usage

``` c++
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "lib/exposer.h"
#include "lib/registry.h"

int main(int argc, char** argv) {
  using namespace prometheus;

  // create an http server running on port 8080
  auto exposer = Exposer{"127.0.0.1:8080"};

  // create a metrics registry with component=main labels applied to all its
  // metrics
  auto registry = std::make_shared<Registry>();

  // add a new counter family to the registry (families combine values with the
  // same name, but distinct label dimenstions)
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

## Building

There are two supported ways to build
`prometheus-cpp` - [CMake](https://cmake.org)
and [bazel](https://bazel.io). Both are tested in CI and should work
on master and for all releases.

In case these instructions don't work for you, looking at
the [travis build script](.travis.yml) might help.

### via CMake

One prerequisite for performing the build using CMake is
having [Protocol Buffers](https://github.com/google/protobuf) >= 3.0
installed. See the [travis build script](.travis.yml) for how to build
it from source, or use your operating systems package manager to
install it.

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
this repo to your project as a dependency. Unfortunately some of the
direct and transitive dependencies do not provide bazel files. You need
to add the following to your WORKSPACE:

```
new_git_repository(
    name = "prometheus_client_model",
    remote = "https://github.com/prometheus/client_model.git",
    commit = "e2da43a",
    build_file_content = """
cc_library(
    name = "prometheus_client_model",
    srcs = [
        "cpp/metrics.pb.cc",
    ],
    hdrs = [
         "cpp/metrics.pb.h",
    ],
    includes = [
         "cpp",
    ],
    visibility = ["//visibility:public"],
    deps = ["@protobuf//:protobuf"],
)
    """,
)

git_repository(
    name = "protobuf",
    remote = "https://github.com/google/protobuf.git",
    tag = "v3.0.0",
    )

new_git_repository(
    name = "civetweb",
    remote = "https://github.com/civetweb/civetweb.git",
    commit = "fbdee74",
    build_file_content = """
cc_library(
    name = "civetweb",
    srcs = [
         "src/civetweb.c",
         "src/CivetServer.cpp",
    ],
    hdrs = [
         "include/civetweb.h",
         "include/CivetServer.h",
         "src/md5.inl",
         "src/handle_form.inl",
    ],
    includes = [
         "include",
    ],
    copts = [
          "-DUSE_IPV6",
          "-DNDEBUG",
          "-DNO_CGI",
          "-DNO_CACHING",
          "-DNO_SSL",
          "-DNO_FILES",
    ],
    visibility = ["//visibility:public"],
)
"""
)

git_repository(
    name = "prometheus_cpp",
    remote = "https://github.com/jupp0r/prometheus-cpp.git",
    commit = "9c865b1c1a4234fa063e91225bb228111ee922ac",
    )
```

Then, you can reference this library in your own BUILD file, as
demonstrated with the sample server included in this repository:

```
cc_binary(
    name = "sample_server",
    srcs = ["sample_server.cc"],
    deps = ["@prometheus_cpp//lib:prometheus-cpp"],
)
```

## Contributing

You can check out this repo and build the library using
``` bash
bazel build //:prometheus-cpp
```

Run the unit tests using
```
bazel test //tests:prometheus_test
```

There is also an integration test that
uses [telegraf](https://github.com/influxdata/telegraf) to scrape a
sample server. With telegraf installed, it can be run using
```
bazel test //tests/integration:scrape_test
```

## Benchmarks

There's a benchmark suite you can run:

```
bazel run -c opt tests/benchmark/benchmarks

INFO: Found 1 target...
Target //tests/benchmark:benchmarks up-to-date:
  bazel-bin/tests/benchmark/benchmarks
INFO: Elapsed time: 1.682s, Critical Path: 1.56s

INFO: Running command line: bazel-bin/tests/benchmark/benchmarks
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
Alpha

* parts of the library are instrumented by itself (bytes scraped,
  number of scrapes, scrape request latencies)
* there is a working [example](tests/integration/sample_server.cc)
  that prometheus successfully scrapes
* gauge, counter and histogram metrics are implemented, summaries
  aren't

## FAQ

### Why do you not provide a `protobuf` version as a submodule in `3rdparty`?

We opted against the submodule solution for protobuf, because
otherwise ABI compatibiliy issues would force all consumers of
`prometheus-cpp` to use exactly the same protobuf version as the one
inside the submodule if they were using protobuf on its own.

To phrase it differently, this library should not control the exact
version, but the executable linking against it should determine a
version that other libraries also link against.

## License

MIT
