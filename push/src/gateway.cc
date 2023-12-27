
#include "prometheus/gateway.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>

#include "detail/curl_wrapper.h"
#include "detail/label_encoder.h"
#include "prometheus/detail/future_std.h"
#include "prometheus/iovector.h"
#include "prometheus/metric_family.h"  // IWYU pragma: keep
#include "prometheus/text_serializer.h"

// IWYU pragma: no_include <system_error>
// IWYU pragma: no_include <cxxabi.h>

namespace prometheus {

Gateway::Gateway(const std::string& host, const std::string& port,
                 const std::string& jobname, const Labels& labels,
                 const std::string& username, const std::string& password,
                 std::chrono::seconds timeout)
    : timeout_(timeout) {
  curlWrapper_ = detail::make_unique<detail::CurlWrapper>(username, password);

  std::stringstream jobUriStream;
  jobUriStream << host << ':' << port << "/metrics";
  detail::encodeLabel(jobUriStream, {"job", jobname});
  jobUri_ = jobUriStream.str();

  std::stringstream labelStream;
  for (auto& label : labels) {
    detail::encodeLabel(labelStream, label);
  }
  labels_ = labelStream.str();
}

Gateway::~Gateway() = default;

Labels Gateway::GetInstanceLabel(std::string hostname) {
  if (hostname.empty()) {
    return Labels{};
  }
  return Labels{{"instance", hostname}};
}

void Gateway::RegisterCollectable(const std::weak_ptr<Collectable>& collectable,
                                  const Labels* labels) {
  std::stringstream ss;

  if (labels) {
    for (auto& label : *labels) {
      ss << "/" << label.first << "/" << label.second;
    }
  }

  std::lock_guard<std::mutex> lock{mutex_};
  CleanupStalePointers(collectables_);
  collectables_.emplace_back(collectable, ss.str());
}

std::string Gateway::getUri(const CollectableEntry& collectable) const {
  std::stringstream uri;
  uri << jobUri_ << labels_ << collectable.second;

  return uri.str();
}

int Gateway::Push() { return push(detail::HttpMethod::Post); }

int Gateway::PushAdd() { return push(detail::HttpMethod::Put); }

int Gateway::push(detail::HttpMethod method) {
  std::lock_guard<std::mutex> lock{mutex_};
  for (auto& wcollectable : collectables_) {
    auto collectable = wcollectable.first.lock();
    if (!collectable) {
      continue;
    }

    IOVector body;
    const auto serializer = TextSerializer{body};

    collectable->Collect(serializer);
    auto uri = getUri(wcollectable);
    auto status_code =
        curlWrapper_->performHttpRequest(method, uri, body, timeout_.count());

    if (status_code < 100 || status_code >= 400) {
      return status_code;
    }
  }

  return 200;
}

std::future<int> Gateway::AsyncPush() {
  return async_push(detail::HttpMethod::Post);
}

std::future<int> Gateway::AsyncPushAdd() {
  return async_push(detail::HttpMethod::Put);
}

std::future<int> Gateway::async_push(detail::HttpMethod method) {
  std::vector<std::future<int>> futures;

  std::lock_guard<std::mutex> lock{mutex_};
  for (auto& wcollectable : collectables_) {
    IOVector body;
    const auto serializer = TextSerializer{body};

    auto collectable = wcollectable.first.lock();
    if (!collectable) {
      continue;
    }

    collectable->Collect(serializer);
    auto uri = getUri(wcollectable);

    futures.push_back(std::async(std::launch::async, [method, uri, body, this] {
      return curlWrapper_->performHttpRequest(method, uri, body,
                                              timeout_.count());
    }));
  }

  const auto reduceFutures = [](std::vector<std::future<int>> lfutures) {
    auto final_status_code = 200;

    for (auto& future : lfutures) {
      auto status_code = future.get();

      if (status_code < 100 || status_code >= 400) {
        final_status_code = status_code;
      }
    }

    return final_status_code;
  };

  return std::async(std::launch::async, reduceFutures, std::move(futures));
}

int Gateway::Delete() {
  return curlWrapper_->performHttpRequest(detail::HttpMethod::Delete, jobUri_,
                                          {}, timeout_.count());
}

std::future<int> Gateway::AsyncDelete() {
  return std::async(std::launch::async, [&] { return Delete(); });
}

int Gateway::DeleteForInstance() {
  return curlWrapper_->performHttpRequest(
      detail::HttpMethod::Delete, jobUri_ + labels_, {}, timeout_.count());
}

std::future<int> Gateway::AsyncDeleteForInstance() {
  return std::async(std::launch::async, [&] { return DeleteForInstance(); });
}

void Gateway::CleanupStalePointers(
    std::vector<CollectableEntry>& collectables) {
  collectables.erase(
      std::remove_if(std::begin(collectables), std::end(collectables),
                     [](const CollectableEntry& candidate) {
                       return candidate.first.expired();
                     }),
      std::end(collectables));
}

bool Gateway::AddHttpHeader(const std::string& header) {
  return curlWrapper_->addHttpHeader(header);
}

}  // namespace prometheus
