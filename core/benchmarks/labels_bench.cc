#include <benchmark/benchmark.h>

#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "benchmark_helpers.h"

#if defined(__cpp_lib_string_view) && defined(__cpp_lib_generic_associative_lookup)
#include <string_view>

using Plain = std::map<std::string, std::string>;
using Transparent = std::map<std::string, std::string, std::less<>>;

template <typename Map>
static Map MakeMap(std::size_t size, std::size_t key_length,
                   std::vector<std::string>& keys) {
  Map map;
  keys.reserve(size);
  for (std::size_t i = 0; i < size; ++i) {
    auto key = GenerateRandomString(key_length);
    map.emplace(key, "value");
    keys.push_back(std::move(key));
  }
  return map;
}

template <typename Map, bool Heterogeneous>
static void BM_Labels_LookupView(benchmark::State& state) {
  std::vector<std::string> keys;
  const auto map = MakeMap<Map>(state.range(0), state.range(1), keys);
  std::size_t i = 0;
  while (state.KeepRunning()) {
    const std::string_view key = keys[i];
    if (++i == keys.size()) i = 0;
    if constexpr (Heterogeneous) {
      benchmark::DoNotOptimize(map.find(key));
    } else {
      benchmark::DoNotOptimize(map.find(std::string(key)));
    }
  }
}

template <typename Map>
static void BM_Labels_LookupString(benchmark::State& state) {
  std::vector<std::string> keys;
  const auto map = MakeMap<Map>(state.range(0), state.range(1), keys);
  std::size_t i = 0;
  while (state.KeepRunning()) {
    const std::string& key = keys[i];
    if (++i == keys.size()) i = 0;
    benchmark::DoNotOptimize(map.find(key));
  }
}

BENCHMARK_TEMPLATE(BM_Labels_LookupView, Plain, false)
    ->ArgsProduct({{4, 16}, {8, 32}});
BENCHMARK_TEMPLATE(BM_Labels_LookupView, Transparent, true)
    ->ArgsProduct({{4, 16}, {8, 32}});
BENCHMARK_TEMPLATE(BM_Labels_LookupString, Plain)
    ->ArgsProduct({{4, 16}, {8, 32}});
BENCHMARK_TEMPLATE(BM_Labels_LookupString, Transparent)
    ->ArgsProduct({{4, 16}, {8, 32}});

#endif
