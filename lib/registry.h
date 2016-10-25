#pragma once

#include <map>
#include <mutex>

#include "collectable.h"
#include "counter_builder.h"
#include "cpp/metrics.pb.h"
#include "family.h"
#include "gauge_builder.h"
#include "histogram.h"
#include "histogram_builder.h"

namespace prometheus {

class Registry : public Collectable {
 public:
  friend class detail::CounterBuilder;
  friend class detail::GaugeBuilder;
  friend class detail::HistogramBuilder;

  Registry() = default;
  Registry(const std::map<std::string, std::string>& const_labels);

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
  std::map<std::string, std::string> const_labels_;
  std::mutex mutex_;
};
}
