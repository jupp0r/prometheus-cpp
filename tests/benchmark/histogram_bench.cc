#include <chrono>
#include <random>

#include <benchmark/benchmark.h>
#include "lib/registry.h"

using prometheus::Histogram;

static Histogram::BucketBoundaries createLinearBuckets(double start, double end,
                                                       double step) {
  auto bucketBoundaries = Histogram::BucketBoundaries{};
  for (auto i = start; i < end; i += step) {
    bucketBoundaries.push_back(i);
  }
  return bucketBoundaries;
}

static void BM_Histogram_Observe(benchmark::State& state) {
  using prometheus::Registry;
  using prometheus::Histogram;

  const auto numberOfBuckets = state.range(0);

  Registry registry{{}};
  auto counterFamily = registry.add_histogram("benchmark histogram", "", {});
  auto bucketBoundaries = createLinearBuckets(0, numberOfBuckets - 1, 1);
  auto histogram = counterFamily->add({}, bucketBoundaries);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> d(0, numberOfBuckets);

  while (state.KeepRunning()) {
    auto observation = d(gen);
    auto start = std::chrono::high_resolution_clock::now();
    histogram->observe(observation);
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}
BENCHMARK(BM_Histogram_Observe)->Range(0, 4096);

static void BM_Histogram_Collect(benchmark::State& state) {
  using prometheus::Registry;
  using prometheus::Histogram;

  const auto numberOfBuckets = state.range(0);

  Registry registry{{}};
  auto counterFamily = registry.add_histogram("benchmark histogram", "", {});
  auto bucketBoundaries = createLinearBuckets(0, numberOfBuckets - 1, 1);
  auto histogram = counterFamily->add({}, bucketBoundaries);

  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(histogram->collect());
  }
}
BENCHMARK(BM_Histogram_Collect)->Range(0, 4096);
