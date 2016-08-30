#include "gmock/gmock.h"

#include "lib/gauge.h"

using namespace testing;
using namespace prometheus;

class GaugeTest : public Test {
  public:
    Gauge gauge_;
};

TEST_F(GaugeTest, initialize_with_zero) {
    EXPECT_EQ(gauge_.value(), 0);
}

TEST_F(GaugeTest, inc) {
    gauge_.inc();
    EXPECT_EQ(gauge_.value(), 1.0);
}

TEST_F(GaugeTest, inc_number) {
    gauge_.inc(4);
    EXPECT_EQ(gauge_.value(), 4.0);
}

TEST_F(GaugeTest, inc_multiple) {
    gauge_.inc();
    gauge_.inc();
    gauge_.inc(5);
    EXPECT_EQ(gauge_.value(), 7.0);
}

TEST_F(GaugeTest, dec) {
    gauge_.set(5.0);
    gauge_.dec();
    EXPECT_EQ(gauge_.value(), 4.0);
}

TEST_F(GaugeTest, dec_number) {
    gauge_.set(5.0);
    gauge_.dec(3.0);
    EXPECT_EQ(gauge_.value(), 2.0);
}

TEST_F(GaugeTest, set) {
    gauge_.set(3.0);
    EXPECT_EQ(gauge_.value(), 3.0);
}

TEST_F(GaugeTest, set_multiple) {
    gauge_.set(3.0);
    gauge_.set(8.0);
    gauge_.set(1.0);
    EXPECT_EQ(gauge_.value(), 1.0);
}

TEST_F(GaugeTest, set_to_current_time) {
    gauge_.set_to_current_time();
    EXPECT_THAT(gauge_.value(), Gt(0.0));
}
