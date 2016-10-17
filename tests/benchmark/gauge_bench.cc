#include <benchmark/benchmark.h>
#include "lib/registry.h"

static void BM_Gauge_Increment(benchmark::State& state) {
  using prometheus::Registry;
  using prometheus::Gauge;
  auto registry = Registry{{}};
  auto gaugeFamily = registry.add_gauge("benchmark gauge", "", {});
  auto gauge = gaugeFamily->add({});

  while (state.KeepRunning()) gauge->inc(2);
}
BENCHMARK(BM_Gauge_Increment);

static void BM_Gauge_Decrement(benchmark::State& state) {
    using prometheus::Registry;
    using prometheus::Gauge;
    auto registry = Registry{{}};
    auto gaugeFamily = registry.add_gauge("benchmark gauge", "", {});
    auto gauge = gaugeFamily->add({});

    while (state.KeepRunning()) gauge->dec(2);
}
BENCHMARK(BM_Gauge_Decrement);

static void BM_Gauge_SetToCurrentTime(benchmark::State& state) {
    using prometheus::Registry;
    using prometheus::Gauge;
    auto registry = Registry{{}};
    auto gaugeFamily = registry.add_gauge("benchmark gauge", "", {});
    auto gauge = gaugeFamily->add({});

    while (state.KeepRunning()) gauge->set_to_current_time();
}
BENCHMARK(BM_Gauge_SetToCurrentTime);

static void BM_Gauge_Collect(benchmark::State& state) {
  using prometheus::Registry;
  using prometheus::Gauge;
  auto registry = Registry{{}};
  auto gaugeFamily = registry.add_gauge("benchmark gauge", "", {});
  auto gauge = gaugeFamily->add({});

  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(gauge->collect());
  };
}
BENCHMARK(BM_Gauge_Collect);
