#include <chrono>
#include <string>
#include <thread>

#include "cpp/metrics.pb.h"

#include "exposer.h"

namespace prometheus {

Exposer::Exposer(const std::string& bindAddress)
    : server_({"listening_ports", bindAddress.c_str()}),
      exposerRegistry_(
          std::make_shared<Registry>(std::map<std::string, std::string>{})),
      metricsHandler_(collectables_, *exposerRegistry_) {
  registerCollectable(exposerRegistry_);
  server_.addHandler("/metrics", &metricsHandler_);
}

void Exposer::registerCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  collectables_.push_back(collectable);
}
}
