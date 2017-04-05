#include <algorithm>
#include <iterator>
#include <numeric>

#include "prometheus/histogram.h"

namespace prometheus {

Histogram::Histogram(const BucketBoundaries& buckets)
    : bucket_boundaries_(buckets), bucket_counts_(buckets.size() + 1) {}

void Histogram::Observe(double value) {
  // TODO: determine bucket list size at which binary search would be faster
  auto bucket_index = static_cast<std::size_t>(std::distance(
      bucket_boundaries_.begin(),
      std::find_if(bucket_boundaries_.begin(), bucket_boundaries_.end(),
                   [value](double boundary) { return boundary > value; })));
  sum_.Increment(value);
  bucket_counts_[bucket_index].Increment();
}

io::prometheus::client::Metric Histogram::Collect() {
  auto metric = io::prometheus::client::Metric{};
  auto histogram = metric.mutable_histogram();

  auto sample_count = std::accumulate(
      bucket_counts_.begin(), bucket_counts_.end(), double{0},
      [](double sum, const Counter& counter) { return sum + counter.Value(); });
  histogram->set_sample_count(sample_count);
  histogram->set_sample_sum(sum_.Value());

  auto cumulative_count = 0ULL;
  for (std::size_t i = 0; i < bucket_counts_.size(); i++) {
    cumulative_count += bucket_counts_[i].Value();
    auto bucket = histogram->add_bucket();
    bucket->set_cumulative_count(cumulative_count);
    bucket->set_upper_bound(i == bucket_boundaries_.size()
                                ? std::numeric_limits<double>::infinity()
                                : bucket_boundaries_[i]);
  }
  return metric;
}
}
