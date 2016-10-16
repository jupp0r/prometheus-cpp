#include <algorithm>
#include <numeric>

#include "histogram.h"

namespace prometheus {

Histogram::Histogram(const BucketBoundaries& buckets)
    : bucketBoundaries_{buckets}, bucketCounts_(buckets.size() + 1) {}

void Histogram::observe(double value) {
  // TODO: determine bucket list size at which binary search would be faster
  auto bucketIndex = std::max(
      0L, std::find_if(bucketBoundaries_.begin(), bucketBoundaries_.end(),
                       [value](double boundary) { return boundary > value; }) -
              bucketBoundaries_.begin());
  sum_.inc(value);
  bucketCounts_[bucketIndex].inc();
}

io::prometheus::client::Metric Histogram::collect() {
  auto metric = io::prometheus::client::Metric{};
  auto histogram = metric.mutable_histogram();

  auto sampleCount = std::accumulate(
      bucketCounts_.begin(), bucketCounts_.end(), double{0},
      [](double sum, const Counter& counter) { return sum + counter.value(); });
  histogram->set_sample_count(sampleCount);
  histogram->set_sample_sum(sum_.value());

  for (int i = 0; i < bucketCounts_.size(); i++) {
    auto& count = bucketCounts_[i];
    auto bucket = histogram->add_bucket();
    bucket->set_cumulative_count(count.value());
    bucket->set_upper_bound(i == bucketBoundaries_.size()
                                ? std::numeric_limits<double>::infinity()
                                : bucketBoundaries_[i]);
  }
  return metric;
}
}
