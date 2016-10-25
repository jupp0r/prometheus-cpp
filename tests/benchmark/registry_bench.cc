#include <chrono>

#include <benchmark/benchmark.h>
#include "lib/registry.h"

#include "benchmark_helpers.h"

static void BM_Registry_CreateFamily(benchmark::State& state) {
  using prometheus::Registry;
  using prometheus::Counter;
  using prometheus::BuildCounter;
  Registry registry;

  while (state.KeepRunning())
    BuildCounter().Name("benchmark counter").Help("").Register(registry);
}
BENCHMARK(BM_Registry_CreateFamily);

static void BM_Registry_CreateCounter(benchmark::State& state) {
  using prometheus::Registry;
  using prometheus::Counter;
  using prometheus::BuildCounter;
  Registry registry;
  auto& counter_family = BuildCounter()
                           .Labels(GenerateRandomLabels(10))
                           .Name("benchmark counter")
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
