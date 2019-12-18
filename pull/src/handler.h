#pragma once

#include <memory>
#include <vector>

#include "CivetServer.h"
#include "prometheus/counter.h"
#include "prometheus/registry.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace detail {
class MetricsHandler : public CivetHandler {
 public:
  MetricsHandler(const std::vector<std::weak_ptr<Collectable>>& collectables,
                 Registry& registry);

  bool handleGet(CivetServer* server, struct mg_connection* conn) override;

 private:
  std::vector<MetricFamily> CollectMetrics() const;

  const std::vector<std::weak_ptr<Collectable>>& collectables_;
  std::shared_ptr<Family<Counter>> bytes_transferred_family_;
  std::shared_ptr<Counter> bytes_transferred_;
  std::shared_ptr<Family<Counter>> num_scrapes_family_;
  std::shared_ptr<Counter> num_scrapes_;
  std::shared_ptr<Family<Summary>> request_latencies_family_;
  std::shared_ptr<Summary> request_latencies_;
};
}  // namespace detail
}  // namespace prometheus
