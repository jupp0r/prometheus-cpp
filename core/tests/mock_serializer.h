#pragma once

#include <gmock/gmock.h>

#include "prometheus/serializer.h"

namespace prometheus {

class MockSerializer : public Serializer {
 public:
  MOCK_METHOD(void, Serialize, (const MetricFamily& family), (const, override));
  MOCK_METHOD(void, Serialize,
              (const MetricFamily& family, const Labels& constantLabels,
               const Labels& metricLabels, const ClientMetric& metric),
              (const, override));
};

}  // namespace prometheus
