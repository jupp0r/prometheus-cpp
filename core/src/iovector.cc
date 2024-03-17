#include "prometheus/iovector.h"

#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>

namespace prometheus {

bool IOVector::empty() const { return data.empty() || !size(); }

std::size_t IOVector::size() const {
  return std::accumulate(begin(data), end(data), std::size_t{0},
                         [](std::size_t size, const ByteVector& chunk) {
                           return size + chunk.size();
                         });
}

std::size_t IOVector::copy(std::size_t offset, void* buffer,
                           std::size_t bufferSize) const {
  std::size_t copied = 0;
  for (const auto& chunk : data) {
    if (offset >= chunk.size()) {
      offset -= chunk.size();
      continue;
    }

    auto chunkSize = std::min(chunk.size() - offset, bufferSize - copied);
    std::copy_n(chunk.data() + offset, chunkSize,
                reinterpret_cast<std::uint8_t*>(buffer) + copied);
    copied += chunkSize;
    offset = 0;

    if (copied == bufferSize) {
      break;
    }
  }
  return copied;
}

void IOVector::add(const std::string& str, std::size_t chunkSize) {
  std::size_t size = str.size();
  std::size_t offset = 0;

  while (size > 0U) {
    if (data.empty() || data.back().size() >= chunkSize) {
      data.emplace_back();
      data.back().reserve(chunkSize);
    }
    auto&& chunk = data.back();
    const auto toAdd = std::min(size, chunkSize - chunk.size());
    chunk.insert(chunk.end(), str.data() + offset, str.data() + offset + toAdd);

    size -= toAdd;
    offset += toAdd;
  }
}
}  // namespace prometheus
