#pragma once

#include <atomic>
#include <cstdint>
#include <memory>

#include "CivetServer.h"

#include "handler.h"
#include "histogram.h"
#include "registry.h"

namespace prometheus {

class Exposer {
 public:
  Exposer(const std::string& bindAddress);
  void registerCollectable(const std::weak_ptr<Collectable>& collectable);

 private:
  CivetServer server_;
  std::vector<std::weak_ptr<Collectable>> collectables_;
  std::shared_ptr<Registry> exposerRegistry_;
  detail::MetricsHandler metricsHandler_;
};
}
