#include "prometheus/detail/ckms_quantiles.h"  // IWYU pragma: export

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>

namespace prometheus {
namespace detail {

CKMSQuantiles::Quantile::Quantile(double quantile, double error)
    : quantile(quantile),
      error(error),
      u(2.0 * error / (1.0 - quantile)),
      v(2.0 * error / quantile) {}

CKMSQuantiles::Item::Item(double value, int lower_delta, int delta)
    : value(value), g(lower_delta), delta(delta) {}

CKMSQuantiles::CKMSQuantiles(const std::vector<Quantile>& quantiles)
    : quantiles_(quantiles), count_(0), buffer_{}, buffer_count_(0) {}

void CKMSQuantiles::insert(double value) {
  buffer_[buffer_count_] = value;
  ++buffer_count_;

  if (buffer_count_ == buffer_.size()) {
    insertBatch();
    compress();
  }
}

double CKMSQuantiles::get(double q) {
  insertBatch();
  compress();

  if (sample_.empty()) {
    return std::numeric_limits<double>::quiet_NaN();
  }

  int rankMin = 0;
  const auto desired = static_cast<int>(q * count_);
  const auto bound = desired + (allowableError(desired) / 2);

  auto it = sample_.begin();
  decltype(it) prev;
  auto cur = it++;

  while (it != sample_.end()) {
    prev = cur;
    cur = it++;

    rankMin += prev->g;

    if (rankMin + cur->g + cur->delta > bound) {
      return prev->value;
    }
  }

  return sample_.back().value;
}

void CKMSQuantiles::reset() {
  count_ = 0;
  sample_.clear();
  buffer_count_ = 0;
}

double CKMSQuantiles::allowableError(int rank) {
  auto size = sample_.size();
  double minError = size + 1;

  for (const auto& q : quantiles_.get()) {
    double error;
    if (rank <= q.quantile * size) {
      error = q.u * (size - rank);
    } else {
      error = q.v * rank;
    }
    if (error < minError) {
      minError = error;
    }
  }

  return minError;
}

bool CKMSQuantiles::insertBatch() {
  // If there is no data to insert return false
  if (buffer_count_ == 0) {
    return false;
  }

  // Sort the buffer upto buffer_count_ to prepare for inserting items
  std::sort(buffer_.begin(), buffer_.begin() + buffer_count_);

  std::size_t start = 0;

  sample_.reserve(buffer_count_);
  // If the sample set is empty, add the first item
  if (sample_.empty()) {
    sample_.emplace_back(buffer_[0], 1, 0);
    ++start;  // Skip the first item since it's already added to the sample
    ++count_;
  }

  // Loop through the buffer and insert the items into the sample set
  for (std::size_t i = start; i < buffer_count_; ++i) {
    float value = buffer_[i];

    auto iterator = std::lower_bound(
        sample_.begin(), sample_.end(), value,
        [](const Item& item, float val) { return item.value < val; });
    std::size_t idx = std::distance(sample_.begin(), iterator);

    int delta = 0;
    if (idx > 0 && idx < sample_.size()) {
      delta = static_cast<int>(
                  std::floor(allowableError(static_cast<int>(idx) + 1))) +
              1;
    }

    sample_.emplace(iterator, value, 1, delta);
    ++count_;
  }

  buffer_count_ = 0;
  return true;
}

void CKMSQuantiles::compress() {
  // If there are less than 2 items in the sample set, there's nothing to
  // compress
  if (sample_.size() < 2) {
    return;
  }

  std::vector<Item> compressed_samples;  // Vector to hold compressed samples
  compressed_samples.reserve(
      sample_.size());  // Reserve space to avoid multiple allocations

  // Start with the first sample
  compressed_samples.push_back(sample_[0]);

  for (std::size_t idx = 1; idx < sample_.size(); ++idx) {
    const Item& current_sample = sample_[idx];
    Item& last_compressed_sample = compressed_samples.back();

    // Check if we can compress the current sample into the last compressed
    // sample
    if (last_compressed_sample.g + current_sample.g + current_sample.delta <=
        allowableError(static_cast<int>(compressed_samples.size()) - 1)) {
      // Merge current sample into last compressed sample
      last_compressed_sample.g += current_sample.g;  // Update weight
    } else {
      // If not compressible, add current sample to compressed samples
      compressed_samples.push_back(current_sample);
    }
  }

  // Replace old samples with new compressed samples
  sample_ = std::move(compressed_samples);
}

}  // namespace detail
}  // namespace prometheus
