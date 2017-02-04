#include <chrono>
#include <string>
#include <thread>

#include "prometheus/exposer.h"

#include "CivetServer.h"
#include "handler.h"
#include "metrics.pb.h"

namespace prometheus {

static const auto uri = std::string{"/metrics"};

Exposer::Exposer(const std::string& bind_address)
    : server_(new CivetServer{{"listening_ports", bind_address.c_str()}}),
      exposer_registry_(std::make_shared<Registry>()),
      metrics_handler_(
          new detail::MetricsHandler{collectables_, *exposer_registry_}) {
  RegisterCollectable(exposer_registry_);
  server_->addHandler(uri, metrics_handler_.get());
}

Exposer::~Exposer() { server_->removeHandler(uri); }

void Exposer::RegisterCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  collectables_.push_back(collectable);
}
}
