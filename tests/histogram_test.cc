#include <limits>

#include "gmock/gmock.h"

#include "lib/histogram.h"

using namespace testing;
using namespace prometheus;

class HistogramTest : public Test {};

TEST_F(HistogramTest, initialize_with_zero) {
    Histogram histogram{{}};
    auto metric = histogram.collect();
    ASSERT_TRUE(metric.has_histogram());
    auto h = metric.histogram();
    EXPECT_EQ(h.sample_count(), 0);
    EXPECT_EQ(h.sample_sum(), 0);
}

TEST_F(HistogramTest, sample_count) {
    Histogram histogram{{1}};
    histogram.observe(0);
    histogram.observe(200);
    auto metric = histogram.collect();
    ASSERT_TRUE(metric.has_histogram());
    auto h = metric.histogram();
    EXPECT_EQ(h.sample_count(), 2);
}

TEST_F(HistogramTest, sample_sum) {
    Histogram histogram{{1}};
    histogram.observe(0);
    histogram.observe(1);
    histogram.observe(101);
    auto metric = histogram.collect();
    ASSERT_TRUE(metric.has_histogram());
    auto h = metric.histogram();
    EXPECT_EQ(h.sample_sum(), 102);
}

TEST_F(HistogramTest, bucket_size) {
    Histogram histogram{{1,2}};
    auto metric = histogram.collect();
    ASSERT_TRUE(metric.has_histogram());
    auto h = metric.histogram();
    EXPECT_EQ(h.bucket_size(), 3);
}

TEST_F(HistogramTest, bucket_count) {
    Histogram histogram{{1,2}};
    histogram.observe(0);
    histogram.observe(0.5);
    histogram.observe(1.5);
    histogram.observe(1.5);
    histogram.observe(3);
    auto metric = histogram.collect();
    ASSERT_TRUE(metric.has_histogram());
    auto h = metric.histogram();
    ASSERT_EQ(h.bucket_size(), 3);
    auto firstBucket = h.bucket(0);
    EXPECT_EQ(firstBucket.cumulative_count(), 2);
    auto secondBucket = h.bucket(1);
    EXPECT_EQ(secondBucket.cumulative_count(), 2);
    auto thirdBucket = h.bucket(2);
    EXPECT_EQ(thirdBucket.cumulative_count(), 1);
}

TEST_F(HistogramTest, bucket_bounds) {
    Histogram histogram{{1,2}};
    auto metric = histogram.collect();
    ASSERT_TRUE(metric.has_histogram());
    auto h = metric.histogram();
    ASSERT_EQ(h.bucket_size(), 3);
    EXPECT_EQ(h.bucket(0).upper_bound(), 1);
    EXPECT_EQ(h.bucket(1).upper_bound(), 2);
    EXPECT_EQ(h.bucket(2).upper_bound(), std::numeric_limits<double>::infinity());
}
