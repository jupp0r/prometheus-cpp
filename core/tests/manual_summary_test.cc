#include "prometheus/manual_summary.h"

#include <gtest/gtest.h>

namespace prometheus {
namespace {

TEST(ManualSummaryTest, initialize_with_zero) {
  ManualSummary summary;
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_count, 0U);
  EXPECT_EQ(s.sample_sum, 0);
  EXPECT_EQ(s.quantile.size(), 0);
}

TEST(ManualSummaryTest, sample_count) {
  ManualSummary summary;
  summary.SetCount(2);
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_count, 2U);
}

TEST(ManualSummaryTest, sample_sum) {
  ManualSummary summary;
  summary.SetSum(100);
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_sum, 100);
}

TEST(ManualSummaryTest, quantile_size) {
  ManualSummary summary;
  summary.AddQuantile(0.5, 12);
  summary.AddQuantile(0.75, 46);
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.quantile.size(), 2U);
}

TEST(ManualSummaryTest, quantile_bounds) {
  ManualSummary summary;
  summary.AddQuantile(0.5, 12);
  summary.AddQuantile(0.75, 46);
  summary.AddQuantile(0.9, 88);
  auto metric = summary.Collect();
  auto s = metric.summary;
  ASSERT_EQ(s.quantile.size(), 3U);
  EXPECT_DOUBLE_EQ(s.quantile.at(0).quantile, 0.5);
  EXPECT_DOUBLE_EQ(s.quantile.at(1).quantile, 0.75);
  EXPECT_DOUBLE_EQ(s.quantile.at(2).quantile, 0.9);
}

TEST(ManualSummaryTest, quantile_values) {
  ManualSummary summary;
  summary.AddQuantile(0.5, 12);
  summary.AddQuantile(0.75, 46);
  summary.AddQuantile(0.9, 88);
  auto metric = summary.Collect();
  auto s = metric.summary;
  ASSERT_EQ(s.quantile.size(), 3U);
  EXPECT_NEAR(s.quantile.at(0).value, 12, 0.01);
  EXPECT_NEAR(s.quantile.at(1).value, 46, 0.01);
  EXPECT_NEAR(s.quantile.at(2).value, 88, 0.01);
}

}  // namespace
}  // namespace prometheus
