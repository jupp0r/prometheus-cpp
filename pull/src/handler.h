#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "civetweb.h"
#include "prometheus/counter.h"
#include "prometheus/registry.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace detail {
class MetricsHandler {
 public:
  explicit MetricsHandler(Registry& registry);

  void RegisterCollectable(const std::weak_ptr<Collectable>& collectable);

  static int requestHandler(struct mg_connection* conn, void* cbdata);

 private:
  bool handleGet(struct mg_connection* conn);

  std::mutex collectables_mutex_;
  std::vector<std::weak_ptr<Collectable>> collectables_;
  Family<Counter>& bytes_transferred_family_;
  Counter& bytes_transferred_;
  Family<Counter>& num_scrapes_family_;
  Counter& num_scrapes_;
  Family<Summary>& request_latencies_family_;
  Summary& request_latencies_;
};
}  // namespace detail
}  // namespace prometheus
