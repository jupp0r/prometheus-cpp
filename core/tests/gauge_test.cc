#include "prometheus/gauge.h"

#include <gmock/gmock.h>

namespace prometheus {
namespace {

TEST(GaugeTest, initialize_with_zero) {
  Gauge gauge;
  EXPECT_EQ(gauge.Value(), 0);
}

TEST(GaugeTest, inc) {
  Gauge gauge;
  gauge.Increment(1, false);
  EXPECT_EQ(gauge.Value(), 1.0);
}

TEST(GaugeTest, inc_number) {
  Gauge gauge;
  gauge.Increment(4, false);
  EXPECT_EQ(gauge.Value(), 4.0);
}

TEST(GaugeTest, inc_multiple) {
  Gauge gauge;
  gauge.Increment(1, false);
  gauge.Increment(1, false);
  gauge.Increment(5, false);
  EXPECT_EQ(gauge.Value(), 7.0);
}

TEST(GaugeTest, inc_negative_value) {
  Gauge gauge;
  gauge.Increment(5.0, false);
  gauge.Increment(-5.0, false);
  EXPECT_EQ(gauge.Value(), 5.0);
}

TEST(GaugeTest, dec) {
  Gauge gauge;
  gauge.Set(5.0, false);
  gauge.Decrement(1, false);
  EXPECT_EQ(gauge.Value(), 4.0);
}

TEST(GaugeTest, dec_negative_value) {
  Gauge gauge;
  gauge.Set(5.0, false);
  gauge.Decrement(-1.0, false);
  EXPECT_EQ(gauge.Value(), 5.0);
}

TEST(GaugeTest, dec_number) {
  Gauge gauge;
  gauge.Set(5.0, false);
  gauge.Decrement(3.0, false);
  EXPECT_EQ(gauge.Value(), 2.0);
}

TEST(GaugeTest, set) {
  Gauge gauge;
  gauge.Set(3.0, false);
  EXPECT_EQ(gauge.Value(), 3.0);
}

TEST(GaugeTest, set_multiple) {
  Gauge gauge;
  gauge.Set(3.0, false);
  gauge.Set(8.0, false);
  gauge.Set(1.0, false);
  EXPECT_EQ(gauge.Value(), 1.0);
}

}  // namespace
}  // namespace prometheus
