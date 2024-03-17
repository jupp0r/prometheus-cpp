#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "prometheus/detail/core_export.h"

namespace prometheus {

struct PROMETHEUS_CPP_CORE_EXPORT IOVector {
  using ByteVector = std::vector<std::uint8_t>;

  bool empty() const;

  std::size_t size() const;

  std::size_t copy(std::size_t offset, void* buffer,
                   std::size_t bufferSize) const;

  void add(const std::string& str, std::size_t chunkSize);

  std::vector<ByteVector> data;
};

}  // namespace prometheus
