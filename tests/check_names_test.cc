#include <gmock/gmock.h>
#include <prometheus/check_names.h>

using namespace testing;
using namespace prometheus;

class CheckNamesTest : public Test {};

TEST_F(CheckNamesTest, empty_metric_name) { EXPECT_FALSE(CheckMetricName("")); }
TEST_F(CheckNamesTest, good_metric_name) {
  EXPECT_TRUE(CheckMetricName("prometheus_notifications_total"));
}
TEST_F(CheckNamesTest, reserved_metric_name) {
  EXPECT_FALSE(CheckMetricName("__some_reserved_metric"));
}

TEST_F(CheckNamesTest, empty_label_name) { EXPECT_FALSE(CheckLabelName("")); }
TEST_F(CheckNamesTest, good_label_name) { EXPECT_TRUE(CheckLabelName("type")); }
TEST_F(CheckNamesTest, reserved_label_name) {
  EXPECT_FALSE(CheckMetricName("__some_reserved_label"));
}
