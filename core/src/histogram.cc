#include "prometheus/histogram.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <numeric>

namespace prometheus {

Histogram::Histogram(const BucketBoundaries& buckets)
    : bucket_boundaries_{buckets}, bucket_counts_{buckets.size() + 1}, sum_{} {
  assert(std::is_sorted(std::begin(bucket_boundaries_),
                        std::end(bucket_boundaries_)));
}

void Histogram::Observe(const double value) {
  // TODO: determine bucket list size at which binary search would be faster
  const auto bucket_index = static_cast<std::size_t>(std::distance(
      bucket_boundaries_.begin(),
      std::find_if(
          std::begin(bucket_boundaries_), std::end(bucket_boundaries_),
          [value](const double boundary) { return boundary >= value; })));
  sum_.Increment(value);
  bucket_counts_[bucket_index].Increment();
}

void Histogram::ObserveMultiple(const std::vector<double> bucket_increments,
                                const double sum_of_values) {
  if (bucket_increments.size() != bucket_counts_.size()) {
    throw std::length_error("The size of bucket_increments was not equal to"
                            "the number of buckets in the histogram.");
  }

  sum_.Increment(sum_of_values);

  for (std::size_t i{0}; i < bucket_counts_.size(); ++i) {
    { bucket_counts_[i].Increment(bucket_increments[i]); }
  }
}

ClientMetric Histogram::Collect() const {
  auto metric = ClientMetric{};

  auto cumulative_count = 0ULL;
  for (std::size_t i{0}; i < bucket_counts_.size(); ++i) {
    cumulative_count += bucket_counts_[i].Value();
    auto bucket = ClientMetric::Bucket{};
    bucket.cumulative_count = cumulative_count;
    bucket.upper_bound = (i == bucket_boundaries_.size()
                              ? std::numeric_limits<double>::infinity()
                              : bucket_boundaries_[i]);
    metric.histogram.bucket.push_back(std::move(bucket));
  }
  metric.histogram.sample_count = cumulative_count;
  metric.histogram.sample_sum = sum_.Value();

  return metric;
}

detail::Builder<Histogram> BuildHistogram() { return {}; }

template <>
Histogram& Family<Histogram>::WithLabelValues(const std::vector<std::string>& values) {\
  return Add(VariableLabels(values), bucket_boundaries_);
}

namespace detail {
template<>
Family<Histogram>& detail::Builder<Histogram>::Register(Registry& registry){
  Family<Histogram>& family = registry.Add<Histogram>(name_, help_, variable_labels_, labels_);
  return family.SetBucketBoundaries(bucket_boundaries_);
}
}  // namespace prometheus
}  // namespace prometheus
