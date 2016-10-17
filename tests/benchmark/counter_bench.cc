#include <benchmark/benchmark.h>
#include "lib/registry.h"

static void BM_Counter_Increment(benchmark::State& state) {
  using prometheus::Registry;
  using prometheus::Counter;
  auto registry = Registry{{}};
  auto counterFamily = registry.add_counter("benchmark counter", "", {});
  auto counter = counterFamily->add({});

  while (state.KeepRunning()) counter->inc();
}
BENCHMARK(BM_Counter_Increment);

static void BM_Counter_Collect(benchmark::State& state) {
  using prometheus::Registry;
  using prometheus::Counter;
  auto registry = Registry{{}};
  auto counterFamily = registry.add_counter("benchmark counter", "", {});
  auto counter = counterFamily->add({});

  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(counter->collect());
  };
}
BENCHMARK(BM_Counter_Collect);
