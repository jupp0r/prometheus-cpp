
#include "prometheus/gateway.h"

#include <memory>
#include <sstream>

#include "prometheus/client_metric.h"
#include "prometheus/serializer.h"
#include "prometheus/text_serializer.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace prometheus {

static const char CONTENT_TYPE[] =
    "text/plain; version=0.0.4; charset=utf-8";

Gateway::Gateway(const std::string host, const std::string service,
                 const std::string jobname, const Labels& labels,
                 const std::string username, const std::string password) : host_{host}, service_{service} {
  std::stringstream jobUriStream;
  jobUriStream << "/metrics/job/" << jobname;
  target_base_ = jobUriStream.str();

  if (!username.empty()) {
    auth_ = username + ":" + password;
  }

  std::stringstream labelStream;
  for (auto& label : labels) {
    labelStream << "/" << label.first << "/" << label.second;
  }
  labels_ = labelStream.str();
}

Gateway::~Gateway() { }

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

int Gateway::performHttpRequest(HttpMethod method, const std::string& target,
                                const std::string& body) const {

  // The io_context is required for all I/O
  boost::asio::io_context ioc;

  // These objects perform our I/O
  boost::asio::ip::tcp::resolver resolver{ioc};
  boost::asio::ip::tcp::socket socket{ioc};

  // Look up the domain name
  auto const results = resolver.resolve(host_, service_);

  // Make the connection on the IP address we get from a lookup
  boost::asio::connect(socket, results.begin(), results.end());

  // Set up an HTTP GET request message
  boost::beast::http::request<boost::beast::http::string_body> req;//{boost::beast::http::verb::get, target, 11};

  req.target(target);
  req.version(11);

  req.set(boost::beast::http::field::host, host_);
  //req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

  if (!body.empty()) {
    req.set(boost::beast::http::field::content_type, CONTENT_TYPE);
    req.body() = body;
  }

  if (!auth_.empty()) {
      std::string encoded;
      encoded.resize(boost::beast::detail::base64::encoded_size(auth_.size()));
      boost::beast::detail::base64::encode(&encoded.front(), auth_.data(), auth_.size());
      req.set(boost::beast::http::field::authorization, "Basic " + encoded);
  }

  switch (method) {
    case HttpMethod::Post:
      req.method(boost::beast::http::verb::post);
      break;

    case HttpMethod::Put:
      req.method(boost::beast::http::verb::put);
      break;

    case HttpMethod::Delete:
      req.method(boost::beast::http::verb::delete_);
      break;
  }

  req.prepare_payload();

  // Send the HTTP request to the remote host
  boost::beast::http::write(socket, req);

  // This buffer is used for reading and must be persisted
  boost::beast::flat_buffer buffer;

  // Declare a container to hold the response
  boost::beast::http::response<boost::beast::http::dynamic_body> res;

  // Receive the HTTP response
  boost::beast::http::read(socket, buffer, res);

  // Gracefully close the socket
  boost::system::error_code ec;
  socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

  return res.result_int();
}

std::string Gateway::getTarget(const CollectableEntry& collectable) const {
  std::stringstream target;
  target << target_base_ << labels_ << collectable.second;

  return target.str();
}

int Gateway::Push() { return push(HttpMethod::Post); }

int Gateway::PushAdd() { return push(HttpMethod::Put); }

int Gateway::push(HttpMethod method) {
  const auto serializer = TextSerializer{};

  for (auto& wcollectable : collectables_) {
    auto collectable = wcollectable.first.lock();
    if (!collectable) {
      continue;
    }

    auto metrics = collectable->Collect();
    auto body = serializer.Serialize(metrics);
    auto target = getTarget(wcollectable);
    auto status_code = performHttpRequest(method, target, body);

    if (status_code < 100 || status_code >= 400) {
      return status_code;
    }
  }

  return 200;
}

std::future<int> Gateway::AsyncPush() { return async_push(HttpMethod::Post); }

std::future<int> Gateway::AsyncPushAdd() { return async_push(HttpMethod::Put); }

std::future<int> Gateway::async_push(HttpMethod method) {
  const auto serializer = TextSerializer{};
  std::vector<std::future<int>> futures;

  for (auto& wcollectable : collectables_) {
    auto collectable = wcollectable.first.lock();
    if (!collectable) {
      continue;
    }

    auto metrics = collectable->Collect();
    auto body = std::make_shared<std::string>(serializer.Serialize(metrics));
    auto uri = getTarget(wcollectable);

    futures.push_back(std::async(std::launch::async, [method, uri, body, this] {
      return performHttpRequest(method, uri, *body);
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
  return performHttpRequest(HttpMethod::Delete, target_base_, {});
}

std::future<int> Gateway::AsyncDelete() {
  return std::async(std::launch::async, [&] { return Delete(); });
}

}  // namespace prometheus
