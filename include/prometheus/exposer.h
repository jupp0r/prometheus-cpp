#pragma once

#include <atomic>
#include <cstdint>
#include <memory>

#include "histogram.h"
#include "registry.h"

class CivetServer;

namespace prometheus {

namespace detail {
class MetricsHandler;
}  // namespace detail

class Exposer {
 public:
  Exposer(const std::string& bind_address);
  ~Exposer();
  void RegisterCollectable(const std::weak_ptr<Collectable>& collectable);

 private:
  std::unique_ptr<CivetServer> server_;
  std::vector<std::weak_ptr<Collectable>> collectables_;
  std::shared_ptr<Registry> exposer_registry_;
  std::unique_ptr<detail::MetricsHandler> metrics_handler_;
};
}
