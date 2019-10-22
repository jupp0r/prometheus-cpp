#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "prometheus/collectable.h"
#include "prometheus/detail/core_export.h"
#include "prometheus/detail/future_std.h"
#include "prometheus/family.h"
#include "prometheus/metric_family.h"

namespace prometheus {

class Counter;
class Gauge;
class Histogram;
class Summary;

namespace detail {

template <typename T>
class Builder;

}
/// \brief Manages the collection of a number of metrics.
///
/// The Registry is responsible to expose data to a class/method/function
/// "bridge", which returns the metrics in a format Prometheus supports.
///
/// The key class is the Collectable. This has a method - called Collect() -
/// that returns zero or more metrics and their samples. The metrics are
/// represented by the class Family<>, which implements the Collectable
/// interface. A new metric is registered with BuildCounter(), BuildGauge(),
/// BuildHistogram() or BuildSummary().
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class PROMETHEUS_CPP_CORE_EXPORT Registry : public Collectable {
 public:
  /// \brief name Create a new registry.
  Registry();

  /// \brief name Destroys a registry.
  ~Registry();

  /// \brief Returns a list of metrics and their samples.
  ///
  /// Every time the Registry is scraped it calls each of the metrics Collect
  /// function.
  ///
  /// \return Zero or more metrics and their samples.
  std::vector<MetricFamily> Collect() override;

 private:
  template <typename T>
  friend class detail::Builder;

  template <typename T>
  Family<T>& Add(const std::string& name, const std::string& help,
                 const std::vector<std::string>& variable_labels,
                 const std::map<std::string, std::string>& labels);

  std::vector<std::unique_ptr<Collectable>> collectables_;
  std::mutex mutex_;
};

}  // namespace prometheus
