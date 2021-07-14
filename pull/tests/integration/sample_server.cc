#include <iostream>
#include <array>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/exposer.h"
#include "prometheus/family.h"
#include "prometheus/registry.h"

int main() {
  using namespace prometheus;

  // create an http server running on port 8080
  Exposer exposer{"127.0.0.1:8080"};

  // create a metrics registry
  // @note it's the users responsibility to keep the object alive
  auto registry = std::make_shared<Registry>();

  // add a new counter family to the registry (families combine values with the
  // same name, but distinct label dimensions)
  //
  // @note please follow the metric-naming best-practices:
  // https://prometheus.io/docs/practices/naming/
  auto& packet_counter = BuildCounter()
                             .Name("observed_packets_total")
                             .Help("Number of observed packets")
                             .Register(*registry);

  // add and remember dimensional data, incrementing those is very cheap
  auto& tcp_rx_counter =
      packet_counter.Add({{"protocol", "tcp"}, {"direction", "rx"}});
  auto& tcp_tx_counter =
      packet_counter.Add({{"protocol", "tcp"}, {"direction", "tx"}});
  auto& udp_rx_counter =
      packet_counter.Add({{"protocol", "udp"}, {"direction", "rx"}});
  auto& udp_tx_counter =
      packet_counter.Add({{"protocol", "udp"}, {"direction", "tx"}});

  // add a counter whose dimensional data is not known at compile time
  // nevertheless dimensional values should only occur in low cardinality:
  // https://prometheus.io/docs/practices/naming/#labels
  auto& http_requests_counter = BuildCounter()
                                    .Name("http_requests_total")
                                    .Help("Number of HTTP requests")
                                    .Register(*registry);

  auto& gauge_requests_counter = BuildGauge()
                                     .Name("gauge_requests_total")
                                     .Help("Number of gauge requests")
                                     .Register(*registry);

  auto& guage_tx_counter =
      gauge_requests_counter.Add({{"type", "guage"}, {"direction", "tx"}});

  auto& guage_rx_counter =
      gauge_requests_counter.Add({{"type", "guage"}, {"direction", "rx"}});


  auto& histogram_family = BuildHistogram()
                              .Name("name_histogram")
                              .Help("help histogram")
                              .Register(*registry);

  const int number_of_buckets = 10;
  auto bucket_boundaries = Histogram::BucketBoundaries{};
  for (auto i = 0; i < number_of_buckets; i += 1) {
    bucket_boundaries.push_back(i);
  }

  auto& histogram = histogram_family.Add({}, bucket_boundaries);

  // ask the exposer to scrape the registry on incoming HTTP requests
  exposer.RegisterCollectable(registry);

  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    const auto random_value = std::rand();
    std::cout << "random_value: " << random_value << ", random_value%10: "
    << random_value%10 << std::endl;

    if (random_value & 1) tcp_rx_counter.Increment();
    if (random_value & 2) tcp_tx_counter.Increment();
    if (random_value & 4) udp_rx_counter.Increment();
    if (random_value & 8) udp_tx_counter.Increment();

    const std::array<std::string, 4> methods = {"GET", "PUT", "POST", "HEAD"};
    auto method = methods.at(random_value % methods.size());
    // dynamically calling Family<T>.Add() works but is slow and should be
    // avoided
    http_requests_counter.Add({{"method", method}}).Increment();

    guage_tx_counter.SetToCurrentTime();
    guage_rx_counter.Increment();

    histogram.Observe(random_value % 10);
  }
  return 0;
}
