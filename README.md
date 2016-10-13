# Zero-Cost Abstraction Metrics for C++ [![Build Status](https://travis-ci.org/jupp0r/prometheus-cpp.svg?branch=master)](https://travis-ci.org/jupp0r/prometheus-cpp)

This library aims to enable
[Metrics-Driven Development](https://sookocheff.com/post/mdd/mdd/) for
C++ serivices. It implements the
[Prometheus Data Model](https://prometheus.io/docs/concepts/data_model/),
a powerful abstraction on which to collect and expose metrics. We
offer the possibility for metrics to collected by Prometheus, but
other push/pull collections can be added as plugins.

## Project Status
Alpha

* parts of the library are instrumented by itself (bytes scraped, number of scrapes)
* there is a working [example](tests/sample_server.cc) that prometheus successfully scrapes
* gauge and counter metrics are implemented, histograms and summaries aren't
* thread safety is missing in registries and metric families (you'd have to lock access yourself for now)

## License
MIT
