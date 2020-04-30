#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "prometheus/collectable.h"
#include "prometheus/detail/pull_export.h"
#include "prometheus/registry.h"

class CivetServer;

namespace prometheus {

namespace detail {
class MetricsHandler;
}  // namespace detail

class Endpoint;

/**
 * Exposer capable of serving different groups of Collectables
 * on different paths.
 */
class PROMETHEUS_CPP_PULL_EXPORT MultiExposer {
 public:
  MultiExposer(const std::string& bind_address,
               std::vector<std::shared_ptr<Endpoint>> endpoints,
               const std::size_t num_threads = 2);

  MultiExposer(std::vector<std::string> options,
               std::vector<std::shared_ptr<Endpoint>> endpoints);

  virtual ~MultiExposer();

  std::vector<int> GetListeningPorts() const;

 protected:
  std::unique_ptr<CivetServer> server_;
  std::vector<std::shared_ptr<Endpoint>> endpoints_;
};

/**
 * Exposer serving a group of Collectables on a single path.
 *
 * Provides a simpler interface than directly using a MultiExposer with
 * a single Endpoint.
 */
class PROMETHEUS_CPP_PULL_EXPORT Exposer : public MultiExposer {
 public:
  explicit Exposer(const std::string& bind_address,
                   const std::string& uri = std::string("/metrics"),
                   const std::size_t num_threads = 2);
  explicit Exposer(std::vector<std::string> options,
                   const std::string& uri = std::string("/metrics"));

  void RegisterCollectable(const std::weak_ptr<Collectable>& collectable);
};

}  // namespace prometheus
