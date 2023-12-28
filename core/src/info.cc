#include "prometheus/info.h"

namespace prometheus {

InfoMetric Info::Collect() const {
  InfoMetric metric;
  metric.value = 1;
  return metric;
}

}  // namespace prometheus
