#include "prometheus/detail/utils.h"

#include "hash.h"

// IWYU pragma: no_include <string>
// IWYU pragma: no_include <utility>

namespace prometheus {

namespace detail {

std::size_t LabelHasher::operator()(const Labels& labels) const {
  size_t seed = 0;
  for (auto& label : labels) {
    hash_combine(&seed, label.first, label.second);
  }

  return seed;
}

}  // namespace detail

}  // namespace prometheus
