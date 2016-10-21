#include "gmock/gmock.h"

#include "lib/counter.h"

using namespace testing;
using namespace prometheus;

class CounterTest : public Test {};

TEST_F(CounterTest, initialize_with_zero) {
  Counter counter;
  EXPECT_EQ(counter.Value(), 0);
}

TEST_F(CounterTest, inc) {
  Counter counter;
  counter.Increment();
  EXPECT_EQ(counter.Value(), 1.0);
}

TEST_F(CounterTest, inc_number) {
  Counter counter;
  counter.Increment(4);
  EXPECT_EQ(counter.Value(), 4.0);
}

TEST_F(CounterTest, inc_multiple) {
  Counter counter;
  counter.Increment();
  counter.Increment();
  counter.Increment(5);
  EXPECT_EQ(counter.Value(), 7.0);
}
