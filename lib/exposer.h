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
  Exposer(const std::string& bind_address);
  void RegisterCollectable(const std::weak_ptr<Collectable>& collectable);

 private:
  CivetServer server_;
  std::vector<std::weak_ptr<Collectable>> collectables_;
  std::shared_ptr<Registry> exposer_registry_;
  detail::MetricsHandler metrics_handler_;
};
}
