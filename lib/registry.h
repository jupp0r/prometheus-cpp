#pragma once

#include <map>

#include "collectable.h"
#include "cpp/metrics.pb.h"
#include "family.h"

namespace prometheus {

class Counter;
class Gauge;

class Registry : public Collectable {
 public:
  Registry() = default;
  Registry(const std::map<std::string, std::string>& constLabels);
  Family<Counter>* add_counter(
      const std::string& name, const std::string& help,
      const std::map<std::string, std::string>& labels);
  Family<Gauge>* add_gauge(const std::string& name, const std::string& help,
                           const std::map<std::string, std::string>& labels);

  // collectable
  std::vector<io::prometheus::client::MetricFamily> collect() override;

 private:
  std::vector<std::unique_ptr<Collectable>> collectables_;
  std::map<std::string, std::string> constLabels_;
};
}
