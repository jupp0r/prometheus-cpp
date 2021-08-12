#include "prometheus/metric.h"

#include <ctime>

namespace prometheus {

Metric::Metric() : last_update_ts_(std::time(nullptr)) {
}

bool Metric::Expired(time_t current_ts) const {
  static const double retire_time = 600;
  return difftime(current_ts, last_update_ts_) > retire_time;
}

void Metric::UpdateTS() {
  last_update_ts_ = std::time(nullptr);
}

}  // namespace prometheus
