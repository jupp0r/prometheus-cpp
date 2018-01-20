#include "handler.h"
#include "prometheus/json_serializer.h"
#include "prometheus/protobuf_delimited_serializer.h"
#include "prometheus/serializer.h"
#include "prometheus/text_serializer.h"

namespace prometheus {
namespace detail {

MetricsHandler::MetricsHandler(
    const std::vector<std::weak_ptr<Collectable>>& collectables,
    Registry& registry)
    : collectables_(collectables),
      bytes_transferred_family_(
          BuildCounter()
              .Name("exposer_bytes_transferred")
              .Help("bytesTransferred to metrics services")
              .Register(registry)),
      bytes_transferred_(bytes_transferred_family_.Add({})),
      num_scrapes_family_(BuildCounter()
                              .Name("exposer_total_scrapes")
                              .Help("Number of times metrics were scraped")
                              .Register(registry)),
      num_scrapes_(num_scrapes_family_.Add({})),
      request_latencies_family_(
          BuildSummary()
              .Name("exposer_request_latencies")
              .Help("Latencies of serving scrape requests, in milliseconds")
              .Register(registry)),
      request_latencies_(request_latencies_family_.Add(
          {}, Summary::Quantiles{{0.5, 0.05}, {0.9, 0.01}, {0.99, 0.001}})) {}

static std::string GetAcceptedEncoding(struct mg_connection* conn) {
  auto request_info = mg_get_request_info(conn);
  for (int i = 0; i < request_info->num_headers; i++) {
    auto header = request_info->http_headers[i];
    if (std::string{header.name} == "Accept") {
      return {header.value};
    }
  }
  return "";
}

bool MetricsHandler::handleGet(CivetServer* server,
                               struct mg_connection* conn) {
  using namespace io::prometheus::client;

  auto start_time_of_request = std::chrono::steady_clock::now();

  auto accepted_encoding = GetAcceptedEncoding(conn);
  auto metrics = CollectMetrics();

  auto content_type = std::string{};

  auto serializer = std::unique_ptr<Serializer>{};

  if (accepted_encoding.find("application/vnd.google.protobuf") !=
      std::string::npos) {
    serializer.reset(new ProtobufDelimitedSerializer());
    content_type =
        "application/vnd.google.protobuf; "
        "proto=io.prometheus.client.MetricFamily; "
        "encoding=delimited";
  } else if (accepted_encoding.find("application/json") != std::string::npos) {
    serializer.reset(new JsonSerializer());
    content_type = "application/json";
  } else {
    serializer.reset(new TextSerializer());
    content_type = "text/plain";
  }

  auto body = serializer->Serialize(metrics);
  mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n",
            content_type.c_str());
  mg_printf(conn, "Content-Length: %lu\r\n\r\n", static_cast<unsigned long>(body.size()));
  mg_write(conn, body.data(), body.size());

  auto stop_time_of_request = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop_time_of_request - start_time_of_request);
  request_latencies_.Observe(duration.count());

  bytes_transferred_.Increment(body.size());
  num_scrapes_.Increment();
  return true;
}
std::vector<io::prometheus::client::MetricFamily>
MetricsHandler::CollectMetrics() const {
  auto collected_metrics = std::vector<io::prometheus::client::MetricFamily>{};

  for (auto&& wcollectable : collectables_) {
    auto collectable = wcollectable.lock();
    if (!collectable) {
      continue;
    }

    for (auto metric : collectable->Collect()) {
      collected_metrics.push_back(metric);
    }
  }

  return collected_metrics;
}
}
}
