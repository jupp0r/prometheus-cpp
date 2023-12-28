#pragma once

#include <cstddef>

#include "prometheus/detail/core_export.h"
#include "prometheus/iovector.h"
#include "prometheus/metric_family.h"
#include "prometheus/serializer.h"

namespace prometheus {

class PROMETHEUS_CPP_CORE_EXPORT TextSerializer : public Serializer {
 public:
  TextSerializer(IOVector& ioVector);

  void Serialize(const MetricFamily& family) const override;
  void Serialize(const MetricFamily& family, const Labels& constantLabels,
                 const Labels& metricLabels,
                 const ClientMetric& metric) const override;

 private:
  void Add(const std::ostringstream& stream) const;

  IOVector& ioVector_;
  static constexpr std::size_t chunkSize_ = 1 * 1024 * 1024;
};

}  // namespace prometheus
