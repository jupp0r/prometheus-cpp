# Prometheus Client Library for Modern C++ [![Build Status](https://travis-ci.org/jupp0r/prometheus-cpp.svg?branch=master)](https://travis-ci.org/jupp0r/prometheus-cpp)

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
  auto exposer = Exposer{8080};

  // create a metrics registry with component=main labels applied to all its
  // metrics
  auto registry = std::make_shared<Registry>(
      std::map<std::string, std::string>{{"component", "main"}});

  // add a new counter family to the registry (families combine values with the
  // same name, but  distinct labels)
  auto counterFamily = registry->add_counter(
      "time_running_seconds", "How many seconds is this server running?", {});

  // add a counter to the metric
  auto secondCounter = counterFamily->add({});

  // ask the exposer to scrape the registry on incoming scrapes
  exposer.registerCollectable(registry);

  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // increment the counter by one (second)
    secondCounter->inc();
  }
  return 0;
}
```

## Building

Install [bazel](https://www.bazel.io).  Bazel makes it trivial to add
this repo to your project as a dependency. Just add the following to
your WORKSPACE

```
git_repository(
    name = "prometheus-cpp",
    remote = "https://github.com/jupp0r/prometheus-cpp.git",
    branch = "master",
    )
```

You can also check out this repo and build the library using
``` bash
bazel build lib:all
```

or run the unit tests using
```
bazel test //tests:prometheus_test
```

There is also an integration test that
uses [telegraf](https://github.com/influxdata/telegraf) to scrape a
sample server. With telegraf installed, it can be run using
```
bazel test //tests/integration:scrape_test
```

## Project Status
Alpha

* parts of the library are instrumented by itself (bytes scraped,
  number of scrapes, scrape request latencies)
* there is a working [example](tests/integration/sample_server.cc)
  that prometheus successfully scrapes
* gauge, counter and histogram metrics are implemented, summaries
  aren't
* thread safety is missing in registries and metric families (you'd
  have to lock access yourself for now)

## License
MIT
