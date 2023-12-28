#pragma once

#include "prometheus/client_metric.h"
#include "prometheus/detail/core_export.h"
#include "prometheus/labels.h"
#include "prometheus/metric_family.h"

namespace prometheus {

class PROMETHEUS_CPP_CORE_EXPORT Serializer {
 public:
  virtual ~Serializer() = default;

  virtual void Serialize(const MetricFamily& family) const = 0;
  virtual void Serialize(const MetricFamily& family,
                         const Labels& constantLabels,
                         const Labels& metricLabels,
                         const CounterMetric& metric) const = 0;
  virtual void Serialize(const MetricFamily& family,
                         const Labels& constantLabels,
                         const Labels& metricLabels,
                         const GaugeMetric& metric) const = 0;
  virtual void Serialize(const MetricFamily& family,
                         const Labels& constantLabels,
                         const Labels& metricLabels,
                         const InfoMetric& metric) const = 0;
  virtual void Serialize(const MetricFamily& family,
                         const Labels& constantLabels,
                         const Labels& metricLabels,
                         const SummaryMetric& metric) const = 0;
  virtual void Serialize(const MetricFamily& family,
                         const Labels& constantLabels,
                         const Labels& metricLabels,
                         const HistogramMetric& metric) const = 0;
  virtual void Serialize(const MetricFamily& family,
                         const Labels& constantLabels,
                         const Labels& metricLabels,
                         const UntypedMetric& metric) const = 0;
};

}  // namespace prometheus
