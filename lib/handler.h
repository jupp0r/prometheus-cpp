#pragma once

#include <memory>
#include <vector>


#include "CivetServer.h"
#include "prometheus/registry.h"

namespace prometheus {
namespace detail {
class MetricsHandler : public CivetHandler {
 public:
  MetricsHandler(const std::vector<std::weak_ptr<Collectable>>& collectables,
                 Registry& registry);

  bool handleGet(CivetServer* server, struct mg_connection* conn) override;

 private:
  std::vector<io::prometheus::client::MetricFamily> CollectMetrics() const;

  const std::vector<std::weak_ptr<Collectable>>& collectables_;
  Family<Counter>& bytes_transfered_family_;
  Counter& bytes_transfered_;
  Family<Counter>& num_scrapes_family_;
  Counter& num_scrapes_;
  Family<Histogram>& request_latencies_family_;
  Histogram& request_latencies_;
};
}
}
