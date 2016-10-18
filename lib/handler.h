#pragma once

#include <memory>
#include <vector>

#include "CivetServer.h"

#include "collectable.h"
#include "registry.h"

namespace prometheus {
namespace detail {
class MetricsHandler : public CivetHandler {
 public:
  MetricsHandler(const std::vector<std::weak_ptr<Collectable>>& collectables,
                 Registry& registry);

  bool handleGet(CivetServer* server, struct mg_connection* conn);

 private:
  std::vector<io::prometheus::client::MetricFamily> collectMetrics() const;

  const std::vector<std::weak_ptr<Collectable>>& collectables_;
  Family<Counter>* bytesTransferedFamily_;
  Counter* bytesTransfered_;
  Family<Counter>* numScrapesFamily_;
  Counter* numScrapes_;
  Family<Histogram>* requestLatenciesFamily_;
  Histogram* requestLatencies_;
};
}
}
