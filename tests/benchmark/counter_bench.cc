#include <benchmark/benchmark.h>
#include "lib/registry.h"

static void BM_Counter_Increment(benchmark::State& state) {
  using prometheus::Registry;
  using prometheus::Counter;
  Registry registry{{}};
  auto counter_family = registry.AddCounter("benchmark counter", "", {});
  auto counter = counter_family->Add({});

  while (state.KeepRunning()) counter->Increment();
}
BENCHMARK(BM_Counter_Increment);

static void BM_Counter_Collect(benchmark::State& state) {
  using prometheus::Registry;
  using prometheus::Counter;
  Registry registry{{}};
  auto counter_family = registry.AddCounter("benchmark counter", "", {});
  auto counter = counter_family->Add({});

  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(counter->Collect());
  };
}
BENCHMARK(BM_Counter_Collect);
