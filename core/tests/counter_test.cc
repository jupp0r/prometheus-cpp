#include "prometheus/counter.h"

#include <gmock/gmock.h>

#include <thread>
#include <vector>

#include <prometheus/counter.h>

namespace prometheus {
namespace {

TEST(CounterTest, initialize_with_zero) {
  Counter counter;
  EXPECT_EQ(counter.Value(), 0);
}

TEST(CounterTest, inc) {
  Counter counter;
  counter.Increment();
  EXPECT_EQ(counter.Value(), 1.0);
}

TEST(CounterTest, inc_number) {
  Counter counter;
  counter.Increment(4);
  EXPECT_EQ(counter.Value(), 4.0);
}

TEST(CounterTest, inc_multiple) {
  Counter counter;
  counter.Increment();
  counter.Increment();
  counter.Increment(5);
  EXPECT_EQ(counter.Value(), 7.0);
}

TEST(CounterTest, inc_negative_value) {
  Counter counter;
  counter.Increment(5.0);
  counter.Increment(-5.0);
  EXPECT_EQ(counter.Value(), 5.0);
}

TEST(CounterTest, concurrent_writes) {
  Counter counter;
  std::vector<std::thread> threads(std::thread::hardware_concurrency());

  for (auto& thread : threads) {
    thread = std::thread{[&counter]() {
      for (int i{0}; i < 100000; ++i) {
        counter.Increment();
      }
    }};
  }

  for (auto& thread : threads) {
    thread.join();
  }

  EXPECT_EQ(100000 * threads.size(), counter.Value());
}

TEST(CounterTest, concurrent_read_write) {
  Counter counter;
  std::vector<double> values;
  values.reserve(100000);

  std::thread reader{[&counter, &values]() {
    for (int i{0}; i < 100000; ++i) {
      values.push_back(counter.Value());
    }
  }};
  std::thread writer{[&counter]() {
    for (int i{0}; i < 100000; ++i) {
      counter.Increment();
    }
  }};

  reader.join();
  writer.join();

  EXPECT_TRUE(std::is_sorted(std::begin(values), std::end(values)));
}

}  // namespace
}  // namespace prometheus
