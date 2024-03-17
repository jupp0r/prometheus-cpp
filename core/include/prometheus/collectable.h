#pragma once

#include "prometheus/detail/core_export.h"
#include "prometheus/serializer.h"

namespace prometheus {

/// @brief Interface implemented by anything that can be used by Prometheus to
/// collect metrics.
///
/// A Collectable has to be registered for collection. See Registry.
class PROMETHEUS_CPP_CORE_EXPORT Collectable {
 public:
  virtual ~Collectable() = default;

  /// \brief Returns a list of metrics and their samples.
  virtual void Collect(const Serializer& out) const = 0;
};

}  // namespace prometheus
