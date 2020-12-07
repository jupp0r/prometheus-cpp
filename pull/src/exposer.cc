#include "prometheus/exposer.h"

#include <algorithm>
#include <chrono>
#include <iterator>
#include <stdexcept>
#include <string>

#include "civetweb.h"
#include "endpoint.h"
#include "handler.h"
#include "prometheus/client_metric.h"
#include "prometheus/detail/future_std.h"

namespace prometheus {

Exposer::Exposer(const std::string& bind_address, const std::size_t num_threads)
    : Exposer(std::vector<std::string>{"listening_ports", bind_address,
                                       "num_threads",
                                       std::to_string(num_threads)}) {}

Exposer::Exposer(std::vector<std::string> options) {
  // create NULL-terminated option list
  std::vector<const char*> pointers;
  pointers.reserve(options.size() + 1u);
  std::transform(options.begin(), options.end(), std::back_inserter(pointers),
                 [](const std::string& o) { return o.c_str(); });
  pointers.push_back(nullptr);

  server_.reset(mg_start(nullptr, this, pointers.data()), mg_stop);
  if (!server_) {
    throw std::runtime_error(
        "null context when constructing civetweb server. "
        "Possible problem binding to port.");
  }
}

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

std::vector<int> Exposer::GetListeningPorts() const {
  std::vector<struct mg_server_port> server_ports(8);
  for (;;) {
    int size = mg_get_server_ports(server_.get(),
                                   static_cast<int>(server_ports.size()),
                                   server_ports.data());
    if (size < static_cast<int>(server_ports.size())) {
      server_ports.resize(size < 0 ? 0 : size);
      break;
    }
    server_ports.resize(server_ports.size() * 2);
  }

  std::vector<int> ports;
  ports.reserve(server_ports.size());
  std::transform(server_ports.begin(), server_ports.end(),
                 std::back_inserter(ports),
                 [](const mg_server_port& sp) { return sp.port; });

  return ports;
}

detail::Endpoint& Exposer::GetEndpointForUri(const std::string& uri) {
  auto sameUri = [uri](const std::unique_ptr<detail::Endpoint>& endpoint) {
    return endpoint->GetURI() == uri;
  };
  auto it = std::find_if(std::begin(endpoints_), std::end(endpoints_), sameUri);
  if (it != std::end(endpoints_)) {
    return *it->get();
  }

  endpoints_.emplace_back(
      detail::make_unique<detail::Endpoint>(server_.get(), uri));
  return *endpoints_.back().get();
}

}  // namespace prometheus
