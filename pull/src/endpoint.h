#pragma once

#include <memory>
#include <string>
#include <vector>

#include "prometheus/collectable.h"
#include "prometheus/registry.h"

class CivetServer;

namespace prometheus {
namespace detail {
class MetricsHandler;

class Endpoint {
 public:
  explicit Endpoint(CivetServer& server, std::string uri);
  ~Endpoint();

  void RegisterCollectable(const std::weak_ptr<Collectable>& collectable);

  const std::string& GetURI() const;

 private:
  CivetServer& server_;
  const std::string uri_;
  std::vector<std::weak_ptr<Collectable>> collectables_;
  // registry for "meta" metrics about the endpoint itself
  std::shared_ptr<Registry> endpoint_registry_;
  std::unique_ptr<MetricsHandler> metrics_handler_;
};

}  // namespace detail
}  // namespace prometheus
