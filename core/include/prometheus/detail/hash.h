#pragma

#include <functional>

namespace prometheus {

  void hash_combine(std::size_t* seed) {

  }

//TODO(qwang) should we provide an interface for user to
// provide their defined logic of computing hash value?
  template <typename T>
  void hash_combine(std::size_t* seed, const T& value) {
    *seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (*seed << 6) + (seed >> 2);
  }

  template <typename T, typename ... Types>
  void hash_combine(std::size_t* seed, const T& value, const Types&... args) {
    hash_combine(seed, value);
    hash_combine(seed, args...);
  }

  template <typename... Types>
  std::size_t hash_value(const Types&... args) {
    std::size_t seed = 0;
    hash_combine(&seed, args...);
    return seed;
  }

}  // prometheus
