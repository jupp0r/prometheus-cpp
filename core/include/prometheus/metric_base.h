#pragma once


// Local
#include "prometheus/family.h"

// CPP
#include <ctime>
#include <atomic>

namespace prometheus {

/// \brief The base class for all metrics
class PROMETHEUS_CPP_CORE_EXPORT MetricBase {
 public:
  MetricBase(const bool alert_if_no_family = true);

  /// \brief    Prints a message to stderr if the metric doesn't have a family
  bool HasFamily();

  /// \brief    Set's alert_if_no_family_ to true
  void EnableAlertIfNoFamily();

  /// \brief    Set's alert_if_no_family_ to false
  void DisableAlertIfNoFamily();

  /// \brief    Prints a message to stderr if the metric doesn't have a family
  void AlertIfNoFamily();

  /// \brief    Update the metric's retention time
  /// \param    retention_time the value used to set retention_time_
  /// \param    bump if true, also update the metric's last_update time
  void UpdateRetentionTime(const double retention_time, const bool bump = true);

  /// \brief    Check if the metric has expired
  bool IsExpired() const;

  template<typename T>
  friend class Family;

 protected:
  bool has_family_{false}, alert_if_no_family_{true};
  std::atomic<double> retention_time_{1e9};
  std::atomic<std::time_t> last_update_{std::time(nullptr)};
};

}  // namespace prometheus
