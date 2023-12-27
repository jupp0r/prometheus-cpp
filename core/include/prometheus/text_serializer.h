#pragma once

#include <iosfwd>

#include "prometheus/client_metric.h"
#include "prometheus/detail/core_export.h"
#include "prometheus/iovector.h"
#include "prometheus/metric_family.h"
#include "prometheus/serializer.h"

namespace prometheus {

class PROMETHEUS_CPP_CORE_EXPORT TextSerializer : public Serializer {
 public:
  TextSerializer(IOVector& ioVector);

  void SerializeHelp(const MetricFamily& family) const override;
  void SerializeMetrics(const MetricFamily& family,
                        const ClientMetric& metric) const override;

 private:
  void Add(const std::ostringstream& stream) const;

  IOVector& ioVector_;
};

}  // namespace prometheus
