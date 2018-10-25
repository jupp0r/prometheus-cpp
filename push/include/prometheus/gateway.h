#pragma once

#include <future>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "prometheus/registry.h"

namespace prometheus {

class Gateway {
 public:
  using Labels = std::map<std::string, std::string>;

  Gateway(const std::string& uri, const std::string jobname,
          const Labels& labels = {}, const std::string username = {},
          const std::string password = {});

  void RegisterCollectable(const std::weak_ptr<Collectable>& collectable,
                           const Labels* labels = nullptr);

  static const Labels GetInstanceLabel(std::string hostname);

  // Push metrics to the given pushgateway.
  int Push() { return push(PushMode::Replace); }

  std::future<int> AsyncPush() { return async_push(PushMode::Replace); }

  // PushAdd metrics to the given pushgateway.
  int PushAdd() { return push(PushMode::Add); }

  std::future<int> AsyncPushAdd() { return async_push(PushMode::Add); }

  // Delete metrics from the given pushgateway.
  int Delete();

  // Delete metrics from the given pushgateway.
  std::future<int> AsyncDelete();

 private:
  std::string jobUri_;
  std::string labels_;
  std::string username_;
  std::string password_;

  std::vector<std::pair<std::weak_ptr<Collectable>, std::string>> collectables_;

  enum class PushMode {
    Add,
    Replace,
  };

  int push(PushMode mode);

  std::future<int> async_push(PushMode mode);
};

}  // namespace prometheus
