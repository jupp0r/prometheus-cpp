#include "prometheus/endpoint.h"

#include "handler.h"

namespace prometheus {

Endpoint::Endpoint(std::string uri)
    : endpoint_registry_(std::make_shared<Registry>()),
      metrics_handler_(
          new detail::MetricsHandler{collectables_, *endpoint_registry_}),
      uri_(std::move(uri)) {
  RegisterCollectable(endpoint_registry_);
}

Endpoint::~Endpoint() = default;

void Endpoint::RegisterCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  collectables_.push_back(collectable);
}

detail::MetricsHandler* Endpoint::getMetricsHandler() const {
  return metrics_handler_.get();
}

const std::string& Endpoint::getURI() const { return uri_; }

}  // namespace prometheus
