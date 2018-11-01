
#include "prometheus/gateway.h"

#include "prometheus/client_metric.h"
#include "prometheus/serializer.h"
#include "prometheus/text_serializer.h"

#include <cpr/cpr.h>

namespace prometheus {

static const char CONTENT_TYPE[] = "text/plain; version=0.0.4; charset=utf-8";

Gateway::Gateway(const std::string& uri, const std::string jobname,
                 const Labels& labels, const std::string username,
                 const std::string password)
    : username_(username), password_(password) {
  std::stringstream jobUriStream;
  jobUriStream << uri << "/metrics/job/" << jobname;
  jobUri_ = jobUriStream.str();

  std::stringstream labelStream;
  for (auto& label : labels) {
    labelStream << "/" << label.first << "/" << label.second;
  }
  labels_ = labelStream.str();
}

const Gateway::Labels Gateway::GetInstanceLabel(std::string hostname) {
  if (hostname.empty()) {
    return Gateway::Labels{};
  }
  return Gateway::Labels{{"instance", hostname}};
}

void Gateway::RegisterCollectable(const std::weak_ptr<Collectable>& collectable,
                                  const Labels* labels) {
  std::stringstream ss;

  if (labels) {
    for (auto& label : *labels) {
      ss << "/" << label.first << "/" << label.second;
    }
  }

  collectables_.push_back(std::make_pair(collectable, ss.str()));
}

int Gateway::performHttpRequest(HttpMethod method, const std::string& uri,
                                const std::string& body) const {
  cpr::Session session;

  session.SetUrl(cpr::Url{uri});

  if (!body.empty()) {
    session.SetHeader(cpr::Header{{"Content-Type", CONTENT_TYPE}});
    session.SetBody(cpr::Body{body});
  }

  if (!username_.empty()) {
    session.SetAuth(cpr::Authentication{username_, password_});
  }

  auto response = cpr::Response{};

  switch (method) {
    case HttpMethod::Post:
      response = session.Post();
      break;

    case HttpMethod::Put:
      response = session.Put();
      break;

    case HttpMethod::Delete:
      response = session.Delete();
      break;
  }

  return response.status_code;
}

std::string Gateway::getUri(const CollectableEntry& collectable) const {
  std::stringstream uri;
  uri << jobUri_ << labels_ << collectable.second;

  return uri.str();
}

int Gateway::push(HttpMethod method) {
  const auto serializer = TextSerializer{};

  for (auto& wcollectable : collectables_) {
    auto collectable = wcollectable.first.lock();
    if (!collectable) {
      continue;
    }

    auto metrics = collectable->Collect();
    auto body = serializer.Serialize(metrics);
    auto uri = getUri(wcollectable);
    auto status_code = performHttpRequest(method, uri, body);

    if (status_code >= 400) {
      return status_code;
    }
  }

  return 200;
}

std::future<int> Gateway::async_push(HttpMethod method) {
  const auto serializer = TextSerializer{};
  std::vector<std::future<int>> futures;

  for (auto& wcollectable : collectables_) {
    auto collectable = wcollectable.first.lock();
    if (!collectable) {
      continue;
    }

    auto metrics = collectable->Collect();
    auto body = serializer.Serialize(metrics);
    auto uri = getUri(wcollectable);

    futures.push_back(std::async(std::launch::async, [&] {
      return performHttpRequest(method, uri, body);
    }));
  }

  return std::async(std::launch::async, [&] {
    auto final_status_code = 200;

    for (auto& future : futures) {
      auto status_code = future.get();

      if (status_code >= 400) {
        final_status_code = status_code;
      }
    }

    return final_status_code;
  });
}

int Gateway::Delete() {
  return performHttpRequest(HttpMethod::Delete, jobUri_, {});
}

std::future<int> Gateway::AsyncDelete() {
  return std::async(std::launch::async, [&] {
    return Delete();
  });
}

}  // namespace prometheus
