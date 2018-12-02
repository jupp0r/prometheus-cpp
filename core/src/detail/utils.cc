#include "prometheus/detail/utils.h"
#include "prometheus/detail/hash.h"

namespace prometheus {

namespace utils {

std::size_t hash_labels(const std::map<std::string, std::string>& labels) {
  size_t seed = 0;
  for (auto& label : labels) {
    hash_combine(&seed, label.first, label.second);
  }

  return seed;
}

}  // utils

}  // prometheus