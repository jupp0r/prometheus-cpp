#pragma once

#include <memory>
#include <utility>

namespace prometheus {
namespace detail {

#if __cplusplus >= 201402L
using std::make_unique;
#else
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif

}  // namespace detail
}  // namespace prometheus
