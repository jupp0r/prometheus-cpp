#include <sys/param.h>
#include <unistd.h>

#include "prometheus/client_metric.h"
#include "prometheus/gateway.h"
#include "prometheus/serializer.h"
#include "prometheus/text_serializer.h"

namespace prometheus {

const char* CONTENT_TYPE = "text/plain; version=0.0.4; charset=utf-8";

Gateway::Gateway(const std::string& uri, const std::string jobname,
                 const labels_t* labels, const std::string username,
                 const std::string password)
    : uri_(uri), jobname_(jobname), username_(username), password_(password) {
  if (labels) {
    std::stringstream ss;

    if (labels) {
      for (auto& label : *labels) {
        ss << "/" << label.first << "/" << label.second;
      }
    }

    labels_ = ss.str();
  }
}

const Gateway::labels_t Gateway::instance_label(void) {
  char hostname[MAXHOSTNAMELEN] = {0};

  if (gethostname(hostname, MAXHOSTNAMELEN - 1)) {
    hostname[0] = 0;
  }

  return Gateway::labels_t{{"instance", hostname}};
}

void Gateway::RegisterCollectable(const std::weak_ptr<Collectable>& collectable,
                                  const labels_t* labels) {
  std::stringstream ss;

  if (labels) {
    for (auto& label : *labels) {
      ss << "/" << label.first << "/" << label.second;
    }
  }

  collectables_.push_back(std::make_pair(collectable, ss.str()));
}

std::stringstream Gateway::job_uri(void) const {
  std::stringstream ss;

  ss << uri_ << "/metrics/job/" << jobname_;

  return ss;
}

int Gateway::push(bool replacing) {
  for (auto& wcollectable : collectables_) {
    auto collectable = wcollectable.first.lock();
    if (!collectable) {
      continue;
    }

    auto metrics = std::vector<MetricFamily>{};

    for (auto metric : collectable->Collect()) {
      metrics.push_back(metric);
    }

    auto uri = job_uri() << labels_ << wcollectable.second;

    auto serializer = std::unique_ptr<Serializer>{new TextSerializer()};

    auto body = serializer->Serialize(metrics);

    cpr::Session session;

    session.SetUrl(cpr::Url{uri.str()});
    session.SetHeader(cpr::Header{{"Content-Type", CONTENT_TYPE}});
    session.SetBody(cpr::Body{body});

    if (!username_.empty()) {
      session.SetAuth(cpr::Authentication{username_, password_});
    }

    auto res = replacing ? session.Post() : session.Put();

    if (res.status_code >= 400) {
      return res.status_code;
    }
  }

  return 200;
}

std::future<int> Gateway::async_push(bool replacing) {
  std::vector<cpr::AsyncResponse> futures;

  for (auto& wcollectable : collectables_) {
    auto collectable = wcollectable.first.lock();
    if (!collectable) {
      continue;
    }

    auto metrics = std::vector<MetricFamily>{};

    for (auto metric : collectable->Collect()) {
      metrics.push_back(metric);
    }

    auto uri = job_uri() << labels_ << wcollectable.second;

    auto serializer = std::unique_ptr<Serializer>{new TextSerializer()};

    auto body = serializer->Serialize(metrics);

    cpr::Session session;

    session.SetUrl(cpr::Url{uri.str()});
    session.SetHeader(cpr::Header{{"Content-Type", CONTENT_TYPE}});
    session.SetBody(cpr::Body{body});

    if (!username_.empty()) {
      session.SetAuth(cpr::Authentication{username_, password_});
    }

    futures.push_back(std::async(std::launch::async, [&] {
      return replacing ? session.Post() : session.Put();
    }));
  }

  return std::async(std::launch::async, [&] {
    for (auto& future : futures) {
      auto res = future.get();

      if (res.status_code > 400) {
        return res.status_code;
      }
    }

    return 200;
  });
}

int Gateway::Delete(void) {
  auto res = cpr::Delete(cpr::Url{cpr::Url{job_uri().str()}});

  return res.status_code;
}

cpr::AsyncResponse Gateway::AsyncDelete(void) {
  return cpr::DeleteAsync(cpr::Url{job_uri().str()});
}

}  // namespace prometheus
