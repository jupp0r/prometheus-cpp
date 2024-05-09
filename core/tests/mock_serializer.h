#pragma once

#include <gmock/gmock.h>

#include "prometheus/serializer.h"

namespace prometheus {

class MockSerializer : public Serializer {
 public:
  MOCK_METHOD(void, SerializeHelp, (const MetricFamily& family),
              (const, override));
  MOCK_METHOD(void, SerializeMetrics,
              (const MetricFamily& family, const ClientMetric& metric),
              (const, override));
};

}  // namespace prometheus
