#include "endpoint.h"

#include "basic_auth.h"
#include "handler.h"
#include "prometheus/detail/future_std.h"

namespace prometheus {
namespace detail {

Endpoint::Endpoint(mg_context* server, std::string uri)
    : server_(server),
      uri_(std::move(uri)),
      endpoint_registry_(std::make_shared<Registry>()),
      metrics_handler_(
          detail::make_unique<MetricsHandler>(*endpoint_registry_)) {
  RegisterCollectable(endpoint_registry_);
  mg_set_request_handler(server_, uri_.c_str(), MetricsHandler::requestHandler,
                         metrics_handler_.get());
}

Endpoint::~Endpoint() {
  mg_set_request_handler(server_, uri_.c_str(), nullptr, nullptr);
  mg_set_auth_handler(server_, uri_.c_str(), nullptr, nullptr);
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
  mg_set_auth_handler(server_, uri_.c_str(), BasicAuthHandler::authHandler,
                      auth_handler_.get());
}

const std::string& Endpoint::GetURI() const { return uri_; }

}  // namespace detail
}  // namespace prometheus
