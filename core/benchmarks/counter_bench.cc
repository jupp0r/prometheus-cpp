#include <benchmark/benchmark.h>
#include <prometheus/registry.h>

static void BM_Counter_IncrementBaseline(benchmark::State& state) {
  struct {
    void Increment() { v += 1.0; }
    double v;
  } counter;

  for (auto _ : state) {
    counter.Increment();
  }
  benchmark::DoNotOptimize(counter.v);
}
BENCHMARK(BM_Counter_IncrementBaseline);

static void BM_Counter_Increment(benchmark::State& state) {
  using prometheus::BuildCounter;
  using prometheus::Counter;
  using prometheus::Registry;
  Registry registry;
  auto& counter_family =
      BuildCounter().Name("benchmark_counter").Help("").Register(registry);
  auto& counter = counter_family.Add({});

  for (auto _ : state) {
    counter.Increment();
  }
  benchmark::DoNotOptimize(counter.Value());
}
BENCHMARK(BM_Counter_Increment);

class BM_Counter : public benchmark::Fixture {
 protected:
  BM_Counter() { this->ThreadPerCpu(); }

  prometheus::Counter counter{};
};

BENCHMARK_F(BM_Counter, ConcurrentIncrement)
(benchmark::State& state) {
  for (auto _ : state) {
    counter.Increment();
  }
  benchmark::DoNotOptimize(counter.Value());
}

static void BM_Counter_Collect(benchmark::State& state) {
  using prometheus::BuildCounter;
  using prometheus::Counter;
  using prometheus::Registry;
  Registry registry;
  auto& counter_family =
      BuildCounter().Name("benchmark_counter").Help("").Register(registry);
  auto& counter = counter_family.Add({});

  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(counter.Collect());
  };
}
BENCHMARK(BM_Counter_Collect);
