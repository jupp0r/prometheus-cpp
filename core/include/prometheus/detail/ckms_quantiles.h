#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <vector>

#include "prometheus/detail/core_export.h"
#include "prometheus/detail/value_type.h"

namespace prometheus {
namespace detail {

class PROMETHEUS_CPP_CORE_EXPORT CKMSQuantiles {
 public:
  struct PROMETHEUS_CPP_CORE_EXPORT Quantile {
    const detail::value_type quantile;
    const detail::value_type error;
    const detail::value_type u;
    const detail::value_type v;

    Quantile(detail::value_type quantile, detail::value_type error);
  };

 private:
  struct Item {
    /*const*/ detail::value_type value;
    int g;
    /*const*/ int delta;

    explicit Item(detail::value_type value, int lower_delta, int delta);
  };

 public:
  explicit CKMSQuantiles(const std::vector<Quantile>& quantiles);

  void insert(detail::value_type value);
  detail::value_type get(detail::value_type q);
  void reset();

 private:
  detail::value_type allowableError(int rank);
  bool insertBatch();
  void compress();

 private:
  const std::reference_wrapper<const std::vector<Quantile>> quantiles_;

  std::size_t count_;
  std::vector<Item> sample_;
  std::array<detail::value_type, 500> buffer_;
  std::size_t buffer_count_;
};

}  // namespace detail
}  // namespace prometheus
