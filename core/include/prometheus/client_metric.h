#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include "prometheus/detail/core_export.h"

namespace prometheus {

// Common Core Metric

struct PROMETHEUS_CPP_CORE_EXPORT CoreMetric {
  std::int64_t timestamp_ms = 0;
};

  // Counter

struct PROMETHEUS_CPP_CORE_EXPORT CounterMetric : public CoreMetric {
  double value = 0.0;
};

  // Gauge

struct PROMETHEUS_CPP_CORE_EXPORT GaugeMetric : public CoreMetric {
  double value = 0.0;
};

  // Info

struct PROMETHEUS_CPP_CORE_EXPORT InfoMetric : public CoreMetric {
  double value = 1.0;
};

  // Summary

struct PROMETHEUS_CPP_CORE_EXPORT SummaryMetric : public CoreMetric {
  struct Quantile {
    double quantile = 0.0;
    double value = 0.0;
  };

  std::uint64_t sample_count = 0;
  double sample_sum = 0.0;
  std::vector<Quantile> quantile;
};

  // Histogram

struct PROMETHEUS_CPP_CORE_EXPORT HistogramMetric : public CoreMetric {
  struct Bucket {
    std::uint64_t cumulative_count = 0;
    double upper_bound = 0.0;
  };

  std::uint64_t sample_count = 0;
  double sample_sum = 0.0;
  std::vector<Bucket> bucket;
};

  // Untyped

struct PROMETHEUS_CPP_CORE_EXPORT UntypedMetric : public CoreMetric {
  double value = 0;
};

}  // namespace prometheus
