#include "gmock/gmock.h"

#include "lib/counter.h"

using namespace testing;
using namespace prometheus;

class CounterTest : public Test {};

TEST_F(CounterTest, initialize_with_zero) {
    Counter counter;
    EXPECT_EQ(counter.value(), 0);
}

TEST_F(CounterTest, inc) {
    Counter counter;
    counter.inc();
    EXPECT_EQ(counter.value(), 1.0);
}

TEST_F(CounterTest, inc_number) {
    Counter counter;
    counter.inc(4);
    EXPECT_EQ(counter.value(), 4.0);
}

TEST_F(CounterTest, inc_multiple) {
    Counter counter;
    counter.inc();
    counter.inc();
    counter.inc(5);
    EXPECT_EQ(counter.value(), 7.0);
}
