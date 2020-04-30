#include "prometheus/exposer.h"

#include <chrono>
#include <string>
#include <thread>

#include "prometheus/client_metric.h"
#include "prometheus/detail/future_std.h"
#include "prometheus/endpoint.h"

#include "CivetServer.h"
#include "handler.h"

namespace prometheus {

MultiExposer::MultiExposer(const std::string& bind_address,
                           std::vector<std::shared_ptr<Endpoint>> endpoints,
                           const std::size_t num_threads)
    : MultiExposer(
          std::vector<std::string>{"listening_ports", bind_address,
                                   "num_threads", std::to_string(num_threads)},
          std::move(endpoints)) {}

MultiExposer::MultiExposer(std::vector<std::string> options,
                           std::vector<std::shared_ptr<Endpoint>> endpoints)
    : server_(detail::make_unique<CivetServer>(std::move(options))),
      endpoints_(std::move(endpoints)) {
  for (const auto& endpoint : endpoints_) {
    server_->addHandler(endpoint->getURI(), endpoint->getMetricsHandler());
  }
}

MultiExposer::~MultiExposer() {
  for (const auto& endpoint : endpoints_) {
    server_->removeHandler(endpoint->getURI());
  }
}

std::vector<int> MultiExposer::GetListeningPorts() const {
  return server_->getListeningPorts();
}

Exposer::Exposer(const std::string& bind_address, const std::string& uri,
                 const std::size_t num_threads)
    : MultiExposer(bind_address, {std::make_shared<Endpoint>(uri)},
                   num_threads) {}

Exposer::Exposer(std::vector<std::string> options, const std::string& uri)
    : MultiExposer(std::move(options), {std::make_shared<Endpoint>(uri)}) {}

void Exposer::RegisterCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  // Exposer is guaranteed to have a single Endpoint.
  endpoints_.at(0)->RegisterCollectable(collectable);
}

}  // namespace prometheus
