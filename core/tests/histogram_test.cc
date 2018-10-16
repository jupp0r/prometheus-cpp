#include <prometheus/histogram.h>

#include <gmock/gmock.h>

#include <limits>

using namespace testing;
using namespace prometheus;

class HistogramTest : public Test {};

TEST_F(HistogramTest, initialize_with_zero) {
  Histogram histogram{{}};
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.sample_count, 0);
  EXPECT_EQ(h.sample_sum, 0);
}

TEST_F(HistogramTest, sample_count) {
  Histogram histogram{{1}};
  histogram.Observe(0);
  histogram.Observe(200);
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.sample_count, 2);
}

TEST_F(HistogramTest, sample_sum) {
  Histogram histogram{{1}};
  histogram.Observe(0);
  histogram.Observe(1);
  histogram.Observe(101);
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.sample_sum, 102);
}

TEST_F(HistogramTest, bucket_size) {
  Histogram histogram{{1, 2}};
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.bucket.size(), 3);
}

TEST_F(HistogramTest, bucket_bounds) {
  Histogram histogram{{1, 2}};
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  EXPECT_EQ(h.bucket.at(0).upper_bound, 1);
  EXPECT_EQ(h.bucket.at(1).upper_bound, 2);
  EXPECT_EQ(h.bucket.at(2).upper_bound,
            std::numeric_limits<double>::infinity());
}

TEST_F(HistogramTest, bucket_counts_not_reset_by_collection) {
  Histogram histogram{{1, 2}};
  histogram.Observe(1.5);
  histogram.Collect();
  histogram.Observe(1.5);
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  ASSERT_EQ(h.bucket.size(), 3);
  EXPECT_EQ(h.bucket.at(1).cumulative_count, 2);
}

TEST_F(HistogramTest, cumulative_bucket_count) {
  Histogram histogram{{1, 2}};
  histogram.Observe(0);
  histogram.Observe(0.5);
  histogram.Observe(1);
  histogram.Observe(1.5);
  histogram.Observe(1.5);
  histogram.Observe(2);
  histogram.Observe(3);
  auto metric = histogram.Collect();
  auto h = metric.histogram;
  ASSERT_EQ(h.bucket.size(), 3);
  EXPECT_EQ(h.bucket.at(0).cumulative_count, 3);
  EXPECT_EQ(h.bucket.at(1).cumulative_count, 6);
  EXPECT_EQ(h.bucket.at(2).cumulative_count, 7);
}
