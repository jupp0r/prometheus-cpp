#pragma once

#include <map>
#include <mutex>

#include "prometheus/collectable.h"
#include "prometheus/counter_builder.h"
#include "prometheus/family.h"
#include "prometheus/gauge_builder.h"
#include "prometheus/histogram.h"
#include "prometheus/histogram_builder.h"

#include "metrics.pb.h"

namespace prometheus {

class Registry : public Collectable {
 public:
  friend class detail::CounterBuilder;
  friend class detail::GaugeBuilder;
  friend class detail::HistogramBuilder;

  // collectable
  std::vector<io::prometheus::client::MetricFamily> Collect() override;

 private:
  Family<Counter>& AddCounter(const std::string& name, const std::string& help,
                              const std::map<std::string, std::string>& labels);
  Family<Gauge>& AddGauge(const std::string& name, const std::string& help,
                          const std::map<std::string, std::string>& labels);
  Family<Histogram>& AddHistogram(
      const std::string& name, const std::string& help,
      const std::map<std::string, std::string>& labels);

  std::vector<std::unique_ptr<Collectable>> collectables_;
  std::mutex mutex_;
};
}
