#include "endpoint.h"

#include "basic_auth.h"
#include "handler.h"
#include "prometheus/detail/future_std.h"

namespace prometheus {
namespace detail {

Endpoint::Endpoint(CivetServer& server, std::string uri)
    : server_(server),
      uri_(std::move(uri)),
      endpoint_registry_(std::make_shared<Registry>()),
      metrics_handler_(
          detail::make_unique<MetricsHandler>(*endpoint_registry_)) {
  RegisterCollectable(endpoint_registry_);
  server_.addHandler(uri_, metrics_handler_.get());
}

Endpoint::~Endpoint() {
  server_.removeHandler(uri_);
  server_.removeAuthHandler(uri_);
}

void Endpoint::RegisterCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  metrics_handler_->RegisterCollectable(collectable);
}

void Endpoint::RegisterAuth(
    std::function<bool(const std::string&, const std::string&)> authCB,
    const std::string& realm) {
  auth_handler_ =
      detail::make_unique<BasicAuthHandler>(std::move(authCB), realm);
  server_.addAuthHandler(uri_, auth_handler_.get());
}

void Endpoint::RemoveCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  metrics_handler_->RemoveCollectable(collectable);
}

const std::string& Endpoint::GetURI() const { return uri_; }

}  // namespace detail
}  // namespace prometheus
