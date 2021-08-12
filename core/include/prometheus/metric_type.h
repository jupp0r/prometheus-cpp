#pragma once

namespace prometheus {

enum class MetricType {
  Counter,
  Gauge,
  Summary,
  Untyped,
  Histogram,
  ManualSummary,
};

}  // namespace prometheus
