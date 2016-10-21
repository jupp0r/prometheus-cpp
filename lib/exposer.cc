#include <chrono>
#include <string>
#include <thread>

#include "cpp/metrics.pb.h"

#include "exposer.h"

namespace prometheus {

Exposer::Exposer(const std::string& bind_address)
    : server_({"listening_ports", bind_address.c_str()}),
      exposer_registry_(
          std::make_shared<Registry>(std::map<std::string, std::string>{})),
      metrics_handler_(collectables_, *exposer_registry_) {
  RegisterCollectable(exposer_registry_);
  server_.addHandler("/metrics", &metrics_handler_);
}

void Exposer::RegisterCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  collectables_.push_back(collectable);
}
}
