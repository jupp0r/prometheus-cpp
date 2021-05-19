#include "prometheus/histogram.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <numeric>
#include <ostream>

namespace prometheus {

Histogram::Histogram(const BucketBoundaries& buckets, const bool alert_if_no_family)
    : MetricBase(alert_if_no_family), bucket_boundaries_(buckets), bucket_counts_(buckets.size() + 1), sum_(0) {
  assert(std::is_sorted(std::begin(bucket_boundaries_),
                        std::end(bucket_boundaries_)));
}

void Histogram::Reset() {
  for (unsigned int bucket_index = 0; bucket_index < bucket_counts_.size(); ++bucket_index) {
    bucket_counts_[bucket_index] = 0;
  }  
  sum_ = 0;
  last_update_ = std::time(nullptr);
  AlertIfNoFamily();
}

void Histogram::Observe(const double value) {
  // TODO: determine bucket list size at which binary search would be faster
  const auto bucket_index = static_cast<std::size_t>(std::distance(
      bucket_boundaries_.begin(),
      std::find_if(
          std::begin(bucket_boundaries_), std::end(bucket_boundaries_),
          [value](const double boundary) { return boundary >= value; })));
  sum_ = sum_ + value;
  bucket_counts_[bucket_index] = bucket_counts_[bucket_index] + 1;
  last_update_ = std::time(nullptr);
  AlertIfNoFamily();
}

void Histogram::ObserveMultiple(const std::vector<double>& bucket_increments,
                                const double sum_of_values) {
  if (bucket_increments.size() != bucket_counts_.size()) {
    throw std::length_error(
        "The size of bucket_increments was not equal to"
        "the number of buckets in the histogram.");
  }

  sum_ = sum_ + sum_of_values;

  for (std::size_t i{0}; i < bucket_counts_.size(); ++i) {
    bucket_counts_[i] = bucket_counts_[i] + bucket_increments[i];
  }
  last_update_ = std::time(nullptr);
  AlertIfNoFamily();
}

ClientMetric Histogram::Collect() const {
  auto metric = ClientMetric{};

  auto cumulative_count = 0ULL;
  for (std::size_t i{0}; i < bucket_counts_.size(); ++i) {
    cumulative_count += bucket_counts_[i];
    auto bucket = ClientMetric::Bucket{};
    bucket.cumulative_count = cumulative_count;
    bucket.upper_bound = (i == bucket_boundaries_.size()
                              ? std::numeric_limits<double>::infinity()
                              : bucket_boundaries_[i]);
    metric.histogram.bucket.push_back(std::move(bucket));
  }
  metric.histogram.sample_count = cumulative_count;
  metric.histogram.sample_sum = sum_;

  return metric;
}

}  // namespace prometheus
