#include <prometheus/counter.h>
#include <prometheus/endpoint.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

#include <chrono>
#include <memory>
#include <thread>

int main() {
  using namespace prometheus;

  auto endpointA = std::make_shared<Endpoint>("/metricsA");
  auto endpointB = std::make_shared<Endpoint>("/metricsB");

  auto registryA = std::make_shared<Registry>();

  // add a new counter family to the registry (families combine values with the
  // same name, but distinct label dimensions)
  auto& counter_familyA = BuildCounter()
                              .Name("time_running_seconds_total")
                              .Help("How many seconds is this server running?")
                              .Labels({{"label", "foo"}})
                              .Register(*registryA);

  // add a counter to the metric family
  auto& seconds_counterA = counter_familyA.Add(
      {{"another_label", "bar"}, {"yet_another_label", "baz"}});

  // ask the exposer to scrape registryA on incoming scrapes for "/metricsA"
  endpointA->RegisterCollectable(registryA);

  auto registryB = std::make_shared<Registry>();

  auto& counter_familyB = BuildCounter()
                              .Name("other_time_running_seconds_total")
                              .Help("How many seconds has something else been running?")
                              .Labels({{"label", "not_foo"}})
                              .Register(*registryB);

  auto& seconds_counterB = counter_familyB.Add(
      {{"another_label", "not_bar"}, {"yet_another_label", "not_baz"}});

  // This endpoint exposes registryB.
  endpointB->RegisterCollectable(registryB);

  // create an http server running on port 8080
  MultiExposer exposer{"127.0.0.1:8080", {endpointA, endpointB}, 1};

  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // increment the counters by one (second)
    seconds_counterA.Increment();
    seconds_counterB.Increment();
  }
  return 0;
}
