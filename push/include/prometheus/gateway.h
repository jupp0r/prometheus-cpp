#pragma once

#include <future>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "prometheus/detail/push_export.h"
#include "prometheus/registry.h"

namespace prometheus {

class CurlWrapper;

class PROMETHEUS_CPP_PUSH_EXPORT Gateway {
 public:
  using Labels = std::map<std::string, std::string>;

  Gateway(const std::string host, const std::string port,
          const std::string jobname, const Labels& labels = {},
          const std::string username = {}, const std::string password = {});
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
  std::string auth_;
  std::unique_ptr<CurlWrapper> curlWrapper_;
  std::mutex mutex_;

  using CollectableEntry = std::pair<std::weak_ptr<Collectable>, std::string>;
  std::vector<CollectableEntry> collectables_;

  std::string getUri(const CollectableEntry& collectable) const;

  enum class HttpMethod {
    Post,
    Put,
    Delete,
  };

  int performHttpRequest(HttpMethod method, const std::string& uri,
                         const std::string& body);

  int push(HttpMethod method);

  std::future<int> async_push(HttpMethod method);
};

}  // namespace prometheus
