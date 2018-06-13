#pragma once

#include <memory>
#include <sstream>

#include <cpr/cpr.h>

#include "histogram.h"
#include "registry.h"

namespace prometheus {

class Gateway {
 public:
  typedef std::map<std::string, std::string> labels_t;

  explicit Gateway(const std::string& uri, const std::string jobname,
                   const labels_t* labels = nullptr,
                   const std::string username = "",
                   const std::string password = "");
  ~Gateway() {}

  void RegisterCollectable(const std::weak_ptr<Collectable>& collectable,
                           const labels_t* labels = nullptr);

  static const labels_t instance_label(void);

  // Push metrics to the given pushgateway.
  int Push(void) { return push(false); }

  std::future<int> AsyncPush(void) { return async_push(false); }

  // PushAdd metrics to the given pushgateway.
  int PushAdd(void) { return push(true); }

  std::future<int> AsyncPushAdd(void) { return async_push(true); }

  // Delete metrics from the given pushgateway.
  int Delete(void);

  // Delete metrics from the given pushgateway.
  cpr::AsyncResponse AsyncDelete(void);

 private:
  std::vector<std::pair<std::weak_ptr<Collectable>, std::string>> collectables_;
  std::string uri_;
  std::string jobname_;
  std::string labels_;
  std::string username_;
  std::string password_;

  std::stringstream job_uri(void) const;

  int push(bool replacing);

  std::future<int> async_push(bool replacing);
};

}  // namespace prometheus
