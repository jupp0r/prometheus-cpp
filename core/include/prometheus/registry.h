#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <set>

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
  /// \brief How to deal with repeatedly added family names for a type.
  ///
  /// Adding a family with the same name but different types is always an error
  /// and will lead to an exception.
  enum class InsertBehavior {
    /// \brief If a family with the same name and labels already exists return
    /// the existing one. If no family with that name exists create it.
    /// Otherwise throw.
    Merge,
    /// \brief Throws if a family with the same name already exists.
    Throw,
    /// \brief Never merge and always create a new family. This violates the
    /// prometheus specification but was the default behavior in earlier
    /// versions
    NonStandardAppend,
  };

  /// \brief name Create a new registry.
  ///
  /// \param insert_behavior How to handle families with the same name.
  explicit Registry(const InsertBehavior insert_behavior = InsertBehavior::Merge);

  /// \brief name Destroys a registry.
  ~Registry();

  /// \brief Returns a list of metrics and their samples.
  ///
  /// Every time the Registry is scraped it calls each of the metrics Collect
  /// function.
  ///
  /// \return Zero or more metrics and their samples.
  std::vector<MetricFamily> Collect() override;

  bool UpdateRetentionTime(const double retention_time, const std::string& re_name, const std::map<std::string, std::string>& re_labels, const std::set<MetricType>& families = {MetricType::Counter, MetricType::Gauge, MetricType::Summary, MetricType::Histogram}, const bool bump = true, const bool debug = false);

 private:
  template <typename T>
  friend class detail::Builder;

  template <typename T>
  std::vector<std::shared_ptr<Family<T>>>& GetFamilies();

  template <typename T>
  bool NameExistsInOtherType(const std::string& name) const;

  template <typename T>
  std::shared_ptr<Family<T>> Add(
                 const std::string& name, const std::string& help,
                 const std::map<std::string, std::string>& labels,
                 const RetentionBehavior retention_behavior = RetentionBehavior::Keep);

  const InsertBehavior insert_behavior_;
  std::vector<std::shared_ptr<Family<Counter>>> counters_;
  std::vector<std::shared_ptr<Family<Gauge>>> gauges_;
  std::vector<std::shared_ptr<Family<Histogram>>> histograms_;
  std::vector<std::shared_ptr<Family<Summary>>> summaries_;
  std::mutex mutex_;
};

}  // namespace prometheus
