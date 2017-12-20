#include <algorithm>
#include <cassert>
#include <iterator>
#include <numeric>

#include "prometheus/histogram.h"

namespace prometheus {

Histogram::Histogram(const BucketBoundaries& buckets)
  : bucket_boundaries_(buckets), bucket_counts_(buckets.size() + 1) {
    assert(std::is_sorted(std::begin(bucket_boundaries_), std::end(bucket_boundaries_)));
  }

void Histogram::Observe(double value) {
  // TODO: determine bucket list size at which binary search would be faster
  auto bucket_index = static_cast<std::size_t>(std::distance(
      bucket_boundaries_.begin(),
      std::find_if(bucket_boundaries_.begin(), bucket_boundaries_.end(),
                   [value](double boundary) { return boundary >= value; })));
  sum_.Increment(value);
  bucket_counts_[bucket_index].Increment();
}

io::prometheus::client::Metric Histogram::Collect() {
  auto metric = io::prometheus::client::Metric{};
  auto histogram = metric.mutable_histogram();

  auto cumulative_count = 0ULL;
  for (std::size_t i = 0; i < bucket_counts_.size(); i++) {
    cumulative_count += bucket_counts_[i].Value();
    auto bucket = histogram->add_bucket();
    bucket->set_cumulative_count(cumulative_count);
    bucket->set_upper_bound(i == bucket_boundaries_.size()
                                ? std::numeric_limits<double>::infinity()
                                : bucket_boundaries_[i]);
  }
  histogram->set_sample_count(cumulative_count);
  histogram->set_sample_sum(sum_.Value());

  return metric;
}
}
