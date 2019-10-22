#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <vector>

#include "prometheus/detail/core_export.h"

namespace prometheus {
namespace detail {

class PROMETHEUS_CPP_CORE_EXPORT CKMSQuantiles {
 public:
  struct PROMETHEUS_CPP_CORE_EXPORT Quantile {
    double quantile;
    double error;
    double u;
    double v;

    Quantile(double quantile, double error);
  };

 private:
  struct Item {
    /*const*/ double value;
    int g;
    /*const*/ int delta;

    explicit Item(double value, int lower_delta, int delta);
  };

 public:
  explicit CKMSQuantiles(const std::vector<Quantile>& quantiles);

  void insert(double value);
  double get(double q);
  void reset();

 private:
  double allowableError(int rank);
  bool insertBatch();
  void compress();

 private:
  const std::reference_wrapper<const std::vector<Quantile>> quantiles_;

  std::size_t count_;
  std::vector<Item> sample_;
  std::array<double, 500> buffer_;
  std::size_t buffer_count_;
};

}  // namespace detail
}  // namespace prometheus
