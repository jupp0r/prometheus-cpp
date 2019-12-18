#include "prometheus/counter.h"

#include <gmock/gmock.h>

namespace prometheus {
namespace {

TEST(CounterTest, initialize_with_zero) {
  Counter counter;
  EXPECT_EQ(counter.Value(), 0);
}

TEST(CounterTest, inc) {
  Counter counter;
  counter.Increment(1, false);
  EXPECT_EQ(counter.Value(), 1.0);
}

TEST(CounterTest, inc_number) {
  Counter counter;
  counter.Increment(4, false);
  EXPECT_EQ(counter.Value(), 4.0);
}

TEST(CounterTest, inc_multiple) {
  Counter counter;
  counter.Increment(1, false);
  counter.Increment(1, false);
  counter.Increment(5, false);
  EXPECT_EQ(counter.Value(), 7.0);
}

TEST(CounterTest, inc_negative_value) {
  Counter counter;
  counter.Increment(5.0, false);
  counter.Increment(-5.0, false);
  EXPECT_EQ(counter.Value(), 5.0);
}

}  // namespace
}  // namespace prometheus
