#pragma once

#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "prometheus/collectable.h"
#include "prometheus/detail/http_method.h"
#include "prometheus/detail/push_export.h"

namespace prometheus {

namespace detail {
class CurlWrapper;
}

class PROMETHEUS_CPP_PUSH_EXPORT Gateway {
 public:
  using Labels = std::map<std::string, std::string>;

  Gateway(const std::string& host, const std::string& port,
          const std::string& jobname, const Labels& labels = {},
          const std::string& username = {}, const std::string& password = {});

  Gateway(const Gateway&) = delete;
  Gateway(Gateway&&) = delete;
  Gateway& operator=(const Gateway&) = delete;
  Gateway& operator=(Gateway&&) = delete;

  ~Gateway();

  void RegisterCollectable(const std::weak_ptr<Collectable>& collectable,
                           const Labels* labels = nullptr);

  static const Labels GetInstanceLabel(std::string hostname);

  // Push metrics to the given pushgateway.
  int Push();

  std::future<int> AsyncPush();

  // PushAdd metrics to the given pushgateway.
  int PushAdd();

  std::future<int> AsyncPushAdd();

  // Delete metrics from the given pushgateway.
  int Delete();

  // Delete metrics from the given pushgateway.
  std::future<int> AsyncDelete();

 private:
  std::string jobUri_;
  std::string labels_;
  std::unique_ptr<detail::CurlWrapper> curlWrapper_;
  std::mutex mutex_;

  using CollectableEntry = std::pair<std::weak_ptr<Collectable>, std::string>;
  std::vector<CollectableEntry> collectables_;

  std::string getUri(const CollectableEntry& collectable) const;

  int push(detail::HttpMethod method);

  std::future<int> async_push(detail::HttpMethod method);

  static void CleanupStalePointers(std::vector<CollectableEntry>& collectables);
};

}  // namespace prometheus
