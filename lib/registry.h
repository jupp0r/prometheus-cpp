#pragma once

#include <map>
#include <mutex>

#include "collectable.h"
#include "cpp/metrics.pb.h"
#include "family.h"
#include "histogram.h"

namespace prometheus {

class Counter;
class Gauge;

class Registry : public Collectable {
 public:
  Registry() = default;
  Registry(const std::map<std::string, std::string>& const_labels);
  Family<Counter>* AddCounter(const std::string& name, const std::string& help,
                              const std::map<std::string, std::string>& labels);
  Family<Gauge>* AddGauge(const std::string& name, const std::string& help,
                          const std::map<std::string, std::string>& labels);
  Family<Histogram>* AddHistogram(
      const std::string& name, const std::string& help,
      const std::map<std::string, std::string>& labels);

  // collectable
  std::vector<io::prometheus::client::MetricFamily> Collect() override;

 private:
  std::vector<std::unique_ptr<Collectable>> collectables_;
  std::map<std::string, std::string> const_labels_;
  std::mutex mutex_;
};
}
