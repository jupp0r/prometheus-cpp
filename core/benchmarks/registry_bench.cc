#include <benchmark/benchmark.h>

#include <chrono>

#include "benchmark_helpers.h"
#include "prometheus/counter.h"
#include "prometheus/family.h"
#include "prometheus/registry.h"

static void BM_Registry_CreateFamily(benchmark::State& state) {
  using prometheus::BuildCounter;
  using prometheus::Counter;
  using prometheus::Registry;
  Registry registry;

  while (state.KeepRunning())
    BuildCounter().Name("benchmark_counter").Help("").Register(registry);
}
BENCHMARK(BM_Registry_CreateFamily);

static void BM_Registry_CreateCounter(benchmark::State& state) {
  using prometheus::BuildCounter;
  using prometheus::Counter;
  using prometheus::Registry;
  Registry registry;
  auto& counter_family = BuildCounter()
                             .Labels(GenerateRandomLabels(10))
                             .Name("benchmark_counter")
                             .Help("")
                             .Register(registry);

  while (state.KeepRunning()) {
    auto labels = GenerateRandomLabels(state.range(0));

    auto start = std::chrono::high_resolution_clock::now();
    counter_family.Add(labels);
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}
BENCHMARK(BM_Registry_CreateCounter)->Range(0, 4096);

static void BM_Registry_CreateCounter_WithLabelValues(benchmark::State& state) {
  using prometheus::BuildCounter;
  using prometheus::Counter;
  using prometheus::Registry;
  Registry registry;

  const auto& labels = GenerateRandomLabels(state.range(0));
  std::vector<std::string> label_names;
  std::vector<std::string> label_values;
  std::for_each(labels.begin(), labels.end(),
                [&](const std::pair<std::string,std::string>& p){
                  label_names.push_back(p.first);
                  label_values.push_back(p.second);
                });

  auto& counter_family = BuildCounter()
          .Labels(GenerateRandomLabels(10))
          .Name("benchmark_counter")
          .Help("")
          .LabelNamesVec(label_names)
          .Register(registry);

  while (state.KeepRunning()) {
    auto start = std::chrono::high_resolution_clock::now();
    counter_family.WithLabelValues(label_values);
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_seconds =
            std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}
BENCHMARK(BM_Registry_CreateCounter_WithLabelValues)->Range(0, 4096);