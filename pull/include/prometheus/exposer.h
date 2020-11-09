#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "prometheus/collectable.h"
#include "prometheus/detail/pull_export.h"
#include "prometheus/registry.h"

class CivetServer;

namespace prometheus {

namespace detail {
class Endpoint;
class MetricsHandler;
}  // namespace detail

class PROMETHEUS_CPP_PULL_EXPORT Exposer {
 public:
  explicit Exposer(const std::string& bind_address,
                   const std::size_t num_threads = 2);
  explicit Exposer(std::vector<std::string> options);
  ~Exposer();
  void RegisterCollectable(const std::weak_ptr<Collectable>& collectable,
                           const std::string& uri = std::string("/metrics"));

  void RegisterAuth(
      std::function<bool(const std::string&, const std::string&)> authCB,
      const std::string& realm = "Prometheus-cpp Exporter",
      const std::string& uri = std::string("/metrics"));

  void RemoveCollectable(const std::weak_ptr<Collectable>& collectable,
                         const std::string& uri = std::string("/metrics"));
  std::vector<int> GetListeningPorts() const;

 private:
  detail::Endpoint& GetEndpointForUri(const std::string& uri);

  std::unique_ptr<CivetServer> server_;
  std::vector<std::unique_ptr<detail::Endpoint>> endpoints_;
};

}  // namespace prometheus
