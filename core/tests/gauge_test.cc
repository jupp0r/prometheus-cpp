#include <gmock/gmock.h>

#include <prometheus/gauge.h>

using namespace testing;
using namespace prometheus;

class GaugeTest : public Test {
 public:
  Gauge gauge_;
};

TEST_F(GaugeTest, initialize_with_zero) { EXPECT_EQ(gauge_.Value(), 0); }

TEST_F(GaugeTest, inc) {
  gauge_.Increment();
  EXPECT_EQ(gauge_.Value(), 1.0);
}

TEST_F(GaugeTest, inc_number) {
  gauge_.Increment(4);
  EXPECT_EQ(gauge_.Value(), 4.0);
}

TEST_F(GaugeTest, inc_multiple) {
  gauge_.Increment();
  gauge_.Increment();
  gauge_.Increment(5);
  EXPECT_EQ(gauge_.Value(), 7.0);
}

TEST_F(GaugeTest, dec) {
  gauge_.Set(5.0);
  gauge_.Decrement();
  EXPECT_EQ(gauge_.Value(), 4.0);
}

TEST_F(GaugeTest, dec_number) {
  gauge_.Set(5.0);
  gauge_.Decrement(3.0);
  EXPECT_EQ(gauge_.Value(), 2.0);
}

TEST_F(GaugeTest, set) {
  gauge_.Set(3.0);
  EXPECT_EQ(gauge_.Value(), 3.0);
}

TEST_F(GaugeTest, set_multiple) {
  gauge_.Set(3.0);
  gauge_.Set(8.0);
  gauge_.Set(1.0);
  EXPECT_EQ(gauge_.Value(), 1.0);
}

TEST_F(GaugeTest, set_to_current_time) {
  gauge_.SetToCurrentTime();
  EXPECT_THAT(gauge_.Value(), Gt(0.0));
}
