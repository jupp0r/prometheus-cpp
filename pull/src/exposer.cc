#include "prometheus/exposer.h"

#include <chrono>
#include <string>
#include <thread>

#include "CivetServer.h"
#include "endpoint.h"
#include "handler.h"
#include "prometheus/client_metric.h"
#include "prometheus/detail/future_std.h"

namespace prometheus {

Exposer::Exposer(const std::string& bind_address, const std::size_t num_threads)
    : Exposer(std::vector<std::string>{"listening_ports", bind_address,
                                       "num_threads",
                                       std::to_string(num_threads)}) {}

Exposer::Exposer(std::vector<std::string> options)
    : server_(detail::make_unique<CivetServer>(std::move(options))) {}

Exposer::~Exposer() = default;

void Exposer::RegisterCollectable(const std::weak_ptr<Collectable>& collectable,
                                  const std::string& uri) {
  auto& endpoint = GetEndpointForUri(uri);
  endpoint.RegisterCollectable(collectable);
}

void Exposer::RegisterAuth(
    std::function<bool(const std::string&, const std::string&)> authCB,
    const std::string& realm, const std::string& uri) {
  auto& endpoint = GetEndpointForUri(uri);
  endpoint.RegisterAuth(std::move(authCB), realm);
}

void Exposer::RemoveCollectable(const std::weak_ptr<Collectable>& collectable,
                                const std::string& uri) {
  auto& endpoint = GetEndpointForUri(uri);
  endpoint.RemoveCollectable(collectable);
}

std::vector<int> Exposer::GetListeningPorts() const {
  return server_->getListeningPorts();
}

detail::Endpoint& Exposer::GetEndpointForUri(const std::string& uri) {
  auto sameUri = [uri](const std::unique_ptr<detail::Endpoint>& endpoint) {
    return endpoint->GetURI() == uri;
  };
  auto it = std::find_if(std::begin(endpoints_), std::end(endpoints_), sameUri);
  if (it != std::end(endpoints_)) {
    return *it->get();
  }

  endpoints_.emplace_back(detail::make_unique<detail::Endpoint>(*server_, uri));
  return *endpoints_.back().get();
}

}  // namespace prometheus
