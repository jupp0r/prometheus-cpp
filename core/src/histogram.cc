#include "prometheus/histogram.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

namespace prometheus {

namespace {

template <class ForwardIterator>
bool is_strict_sorted(ForwardIterator first, ForwardIterator last) {
  return std::adjacent_find(first, last,
                            std::greater_equal<typename std::iterator_traits<
                                ForwardIterator>::value_type>()) == last;
}

}  // namespace

Histogram::Histogram(const BucketBoundaries& buckets)
    : bucket_boundaries_{buckets} {
  if (!is_strict_sorted(begin(bucket_boundaries_), end(bucket_boundaries_))) {
    throw std::invalid_argument("Bucket Boundaries must be strictly sorted");
  }

  for (std::int32_t i{0}; i < 2; ++i) {
    values_[i].bucket_counts_.resize(buckets.size() + 1);
    values_[i].count_ = 0;
  }

  hot_index_value_count_ = 0;
}

Histogram::Histogram(BucketBoundaries&& buckets)
    : bucket_boundaries_{std::move(buckets)} {
  if (!is_strict_sorted(begin(bucket_boundaries_), end(bucket_boundaries_))) {
    throw std::invalid_argument("Bucket Boundaries must be strictly sorted");
  }

  for (std::int32_t i{0}; i < 2; ++i) {
    values_[i].bucket_counts_.resize(buckets.size() + 1);
    values_[i].count_ = 0;
  }

  hot_index_value_count_ = 0;
}

void Histogram::Observe(const double value) {
  const auto bucket_index = static_cast<std::size_t>(
      std::distance(bucket_boundaries_.begin(),
                    std::lower_bound(bucket_boundaries_.begin(),
                                     bucket_boundaries_.end(), value)));
  std::uint64_t hot_index_value_count = hot_index_value_count_.fetch_add(1) + 1;

  auto& value_now = values_[hot_index_value_count >> 63];

  value_now.sum_.Increment(value);
  value_now.bucket_counts_[bucket_index].Increment();
  value_now.count_.fetch_add(1);
}

void Histogram::ObserveMultiple(const std::vector<double>& bucket_increments,
                                const double sum_of_values) {
  double bucket_increments_sum = 0;

  for (std::size_t i{0}; i < bucket_increments.size(); ++i) {
    bucket_increments_sum += bucket_increments[i];
  }

  std::uint64_t value_count =
      hot_index_value_count_.fetch_add(bucket_increments_sum) +
      bucket_increments_sum;

  auto& value_now = values_[value_count >> 63];

  if (bucket_increments.size() != value_now.bucket_counts_.size()) {
    throw std::length_error(
        "The size of bucket_increments was not equal to"
        "the number of buckets in the histogram.");
  }

  value_now.sum_.Increment(sum_of_values);

  for (std::size_t i{0}; i < value_now.bucket_counts_.size(); ++i) {
    value_now.bucket_counts_[i].Increment(bucket_increments[i]);
  }

  value_now.count_.fetch_add(bucket_increments_sum);
}

void Histogram::Reset() {
  std::lock_guard<std::mutex> lock(mutex_);

  // switch
  std::uint64_t hot_index_value_count =
      hot_index_value_count_.fetch_add(1ULL << 63) + (1ULL << 63);
  auto& value_old = values_[(~hot_index_value_count) >> 63];
  std::uint64_t value_count_real = hot_index_value_count & ((1ULL << 63) - 1);

  // wait for all the Observe and ObserveMultiple completed
  while (value_count_real != value_old.count_) {
    sched_yield();
  }

  // clear data
  value_old.count_.store(0);
  value_old.sum_.Set(0);

  for (std::size_t i{0}; i < value_old.bucket_counts_.size(); ++i) {
    value_old.bucket_counts_[i].Set(0);
  }
}

ClientMetric Histogram::Collect() {
  std::lock_guard<std::mutex> lock(mutex_);

  // switch
  std::uint64_t hot_index_value_count =
      hot_index_value_count_.fetch_add(1ULL << 63) + (1ULL << 63);
  auto& value_new = values_[hot_index_value_count >> 63];
  auto& value_old = values_[(~hot_index_value_count) >> 63];
  std::uint64_t value_count_real = hot_index_value_count & ((1ULL << 63) - 1);

  // wait for all the Observe and ObserveMultiple completed
  while (value_count_real != value_old.count_) {
    sched_yield();
  }

  // output data
  auto metric = ClientMetric{};
  auto cumulative_count = 0ULL;

  metric.histogram.bucket.reserve(value_old.bucket_counts_.size());

  for (std::size_t i{0}; i < value_old.bucket_counts_.size(); ++i) {
    cumulative_count += value_old.bucket_counts_[i].Value();
    auto bucket = ClientMetric::Bucket{};
    bucket.cumulative_count = cumulative_count;
    bucket.upper_bound = (i == bucket_boundaries_.size()
                              ? std::numeric_limits<double>::infinity()
                              : bucket_boundaries_[i]);
    metric.histogram.bucket.push_back(std::move(bucket));
  }

  metric.histogram.sample_count = cumulative_count;
  metric.histogram.sample_sum = value_old.sum_.Value();

  // cumulate data
  value_new.count_.fetch_add(value_old.count_);
  value_new.sum_.Increment(value_old.sum_.Value());

  for (std::size_t i{0}; i < value_old.bucket_counts_.size(); ++i) {
    value_new.bucket_counts_[i].Increment(value_old.bucket_counts_[i].Value());
  }

  // clear data
  value_old.count_.store(0);
  value_old.sum_.Set(0);

  for (std::size_t i{0}; i < value_old.bucket_counts_.size(); ++i) {
    value_old.bucket_counts_[i].Set(0);
  }

  return metric;
}

}  // namespace prometheus
