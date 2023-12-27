#pragma once

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <string>
#include <vector>

#include "prometheus/detail/core_export.h"

namespace prometheus {

struct PROMETHEUS_CPP_CORE_EXPORT IOVector {
  bool empty() const { return data.empty() || !size(); }

  std::size_t size() const {
    return std::accumulate(begin(data), end(data), std::size_t{0},
                           [](std::size_t size, const std::string& chunk) {
                             return size + chunk.size();
                           });
  }

  std::size_t copy(std::size_t offset, char* buffer,
                   std::size_t bufferSize) const {
    std::size_t copied = 0;
    for (const auto& chunk : data) {
      if (offset >= chunk.size()) {
        offset -= chunk.size();
        continue;
      }

      auto chunkSize = std::min(chunk.size() - offset, bufferSize - copied);
      std::copy_n(chunk.data() + offset, chunkSize, buffer + copied);
      copied += chunkSize;
      offset = 0;

      if (copied == bufferSize) {
        break;
      }
    }
    return copied;
  }

  std::vector<std::string> data;
};

}  // namespace prometheus
