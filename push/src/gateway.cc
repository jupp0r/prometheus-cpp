
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

int Gateway::push(PushMode mode) {
  const auto serializer = TextSerializer{};

  for (auto& wcollectable : collectables_) {
    auto collectable = wcollectable.first.lock();
    if (!collectable) {
      continue;
    }

    auto metrics = collectable->Collect();

    std::stringstream uri;
    uri << jobUri_ << labels_ << wcollectable.second;

    auto body = serializer.Serialize(metrics);

    cpr::Session session;

    session.SetUrl(cpr::Url{uri.str()});
    session.SetHeader(cpr::Header{{"Content-Type", CONTENT_TYPE}});
    session.SetBody(cpr::Body{body});

    if (!username_.empty()) {
      session.SetAuth(cpr::Authentication{username_, password_});
    }

    auto res = mode == PushMode::Replace ? session.Post() : session.Put();

    if (res.status_code >= 400) {
      return res.status_code;
    }
  }

  return 200;
}

std::future<int> Gateway::async_push(PushMode mode) {
  const auto serializer = TextSerializer{};
  std::vector<cpr::AsyncResponse> futures;

  for (auto& wcollectable : collectables_) {
    auto collectable = wcollectable.first.lock();
    if (!collectable) {
      continue;
    }

    auto metrics = collectable->Collect();

    std::stringstream uri;
    uri << jobUri_ << labels_ << wcollectable.second;

    auto body = serializer.Serialize(metrics);

    cpr::Session session;

    session.SetUrl(cpr::Url{uri.str()});
    session.SetHeader(cpr::Header{{"Content-Type", CONTENT_TYPE}});
    session.SetBody(cpr::Body{body});

    if (!username_.empty()) {
      session.SetAuth(cpr::Authentication{username_, password_});
    }

    futures.push_back(std::async(std::launch::async, [&] {
      return mode == PushMode::Replace ? session.Post() : session.Put();
    }));
  }

  return std::async(std::launch::async, [&] {
    for (auto& future : futures) {
      auto res = future.get();

      if (res.status_code >= 400) {
        return res.status_code;
      }
    }

    return 200;
  });
}

int Gateway::Delete() {
  auto res = cpr::Delete(cpr::Url{cpr::Url{jobUri_}});

  return res.status_code;
}

std::future<int> Gateway::AsyncDelete() {
  const auto url = cpr::Url{jobUri_};

  return std::async(std::launch::async, [url] {
    auto future = cpr::DeleteAsync(url);
    auto res = future.get();

    if (res.status_code >= 400) {
      return res.status_code;
    }

    return 200;
  });
}

}  // namespace prometheus
