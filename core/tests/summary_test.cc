#include "prometheus/summary.h"

#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <memory>
#include <thread>

namespace prometheus {
namespace {

TEST(SummaryTest, initialize_with_zero) {
  Summary summary{Summary::Quantiles{}};
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_count, 0U);
  EXPECT_EQ(s.sample_sum, 0);
}

TEST(SummaryTest, sample_count) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}}};
  summary.Observe(0);
  summary.Observe(200);
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_count, 2U);
}

TEST(SummaryTest, sample_sum) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}}};
  summary.Observe(0);
  summary.Observe(1);
  summary.Observe(101);
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_sum, 102);
}

TEST(SummaryTest, quantile_size) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}, {0.90, 0.01}}};
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.quantile.size(), 2U);
}

TEST(SummaryTest, quantile_bounds) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}, {0.90, 0.01}, {0.99, 0.001}}};
  auto metric = summary.Collect();
  auto s = metric.summary;
  ASSERT_EQ(s.quantile.size(), 3U);
  EXPECT_DOUBLE_EQ(s.quantile.at(0).quantile, 0.5);
  EXPECT_DOUBLE_EQ(s.quantile.at(1).quantile, 0.9);
  EXPECT_DOUBLE_EQ(s.quantile.at(2).quantile, 0.99);
}

TEST(SummaryTest, quantile_values) {
  static const int SAMPLES = 100000;

  Summary summary{Summary::Quantiles{{0.5, 0.05}, {0.9, 0.01}, {0.99, 0.001}},
                  std::chrono::hours{1}};  // prevent rotation on slow CPUs
  for (int i = 1; i <= SAMPLES; ++i) summary.Observe(i);

  auto metric = summary.Collect();
  auto s = metric.summary;
  ASSERT_EQ(s.quantile.size(), 3U);

  EXPECT_NEAR(s.quantile.at(0).value, 0.5 * SAMPLES, 0.05 * SAMPLES);
  EXPECT_NEAR(s.quantile.at(1).value, 0.9 * SAMPLES, 0.01 * SAMPLES);
  EXPECT_NEAR(s.quantile.at(2).value, 0.99 * SAMPLES, 0.001 * SAMPLES);
}

TEST(SummaryTest, single_quantile_with_ascending_observations) {
  constexpr double quantile = 0.9;
  constexpr double error = 0.01;

  for (const int samples : {100, 500}) {
    SCOPED_TRACE(samples);
    Summary summary{Summary::Quantiles{{quantile, error}},
                    std::chrono::hours{1}};
    for (int i = 1; i <= samples; ++i) summary.Observe(i);

    auto metric = summary.Collect();
    const auto& s = metric.summary;
    ASSERT_EQ(s.quantile.size(), 1U);
    EXPECT_NEAR(s.quantile.at(0).value, quantile * samples, error * samples);
  }
}

TEST(SummaryTest, max_age) {
  Summary summary{Summary::Quantiles{{0.99, 0.001}}, std::chrono::seconds(1),
                  2};
  summary.Observe(8.0);

  const auto test_value = [&summary](double ref) {
    auto metric = summary.Collect();
    auto s = metric.summary;
    ASSERT_EQ(s.quantile.size(), 1U);

    if (std::isnan(ref))
      EXPECT_TRUE(std::isnan(s.quantile.at(0).value));
    else
      EXPECT_DOUBLE_EQ(s.quantile.at(0).value, ref);
  };

  test_value(8.0);
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  test_value(8.0);
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  test_value(std::numeric_limits<double>::quiet_NaN());
}

TEST(SummaryTest, construction_with_dynamic_quantile_vector) {
  auto quantiles = Summary::Quantiles{{0.99, 0.001}};
  quantiles.push_back({0.5, 0.05});

  Summary summary{quantiles, std::chrono::seconds(1), 2};
  summary.Observe(8.0);
}

TEST(SummaryTest, compress_keeps_larger_value_on_merge) {
  // With q=1.0 the two samples are eligible to be merged in compress().
  // The surviving sample must retain the larger value, not the smaller one.
  Summary summary{Summary::Quantiles{{1.0, 0.001}}, std::chrono::hours{1}};
  summary.Observe(1.0);
  summary.Observe(100.0);
  auto metric = summary.Collect();
  auto s = metric.summary;
  ASSERT_EQ(s.quantile.size(), 1U);
  EXPECT_DOUBLE_EQ(s.quantile.at(0).value, 100.0);
}

TEST(SummaryTest, insert_preserves_double_precision) {
  // 2^24+1 is not exactly representable as float, so a truncating
  // double->float->double round trip would corrupt the stored value.
  Summary summary{Summary::Quantiles{{1.0, 0.001}}, std::chrono::hours{1}};
  const double v = 16777217.0;
  summary.Observe(1.0);
  summary.Observe(v);
  auto metric = summary.Collect();
  auto s = metric.summary;
  ASSERT_EQ(s.quantile.size(), 1U);
  EXPECT_DOUBLE_EQ(s.quantile.at(0).value, v);
}

TEST(SummaryTest, quantile_with_out_of_order_batches) {
  // Insert small values into a sample that already contains larger values.
  Summary summary{Summary::Quantiles{{0.5, 0.05}}, std::chrono::hours{1}};

  for (int i = 0; i < 10; ++i) summary.Observe(100.0 + i);
  summary.Collect();  // Flush the first batch before inserting smaller values.

  for (int i = 0; i < 10; ++i) summary.Observe(1.0 + i);
  auto metric = summary.Collect();
  auto s = metric.summary;

  // 20 observations total, sum = (1..10) + (100..109) = 55 + 1045 = 1100
  EXPECT_EQ(s.sample_count, 20U);
  EXPECT_DOUBLE_EQ(s.sample_sum, 1100.0);
  // p50 with error 0.05 permits ranks 9..11: values 9, 10, or 100.
  ASSERT_EQ(s.quantile.size(), 1U);
  const auto value = s.quantile.at(0).value;
  EXPECT_TRUE(value == 9.0 || value == 10.0 || value == 100.0)
      << "Unexpected p50 value: " << value;
}

}  // namespace
}  // namespace prometheus
