#include "handler.h"
#include "json_serializer.h"
#include "protobuf_delimited_serializer.h"
#include "serializer.h"
#include "text_serializer.h"

namespace prometheus {
namespace detail {

MetricsHandler::MetricsHandler(
    const std::vector<std::weak_ptr<Collectable>>& collectables,
    Registry& registry)
    : collectables_(collectables),
      bytesTransferedFamily_(registry.add_counter(
          "exposer_bytes_transfered", "bytesTransferred to metrics services",
          {{"component", "exposer"}})),
      bytesTransfered_(bytesTransferedFamily_->add({})),
      numScrapesFamily_(registry.add_counter(
          "exposer_total_scrapes", "Number of times metrics were scraped",
          {{"component", "exposer"}})),
      numScrapes_(numScrapesFamily_->add({})),
      requestLatenciesFamily_(registry.add_histogram(
          "exposer_request_latencies",
          "Latencies of serving scrape requests, in milliseconds",
          {{"component", "exposer"}})),
      requestLatencies_(requestLatenciesFamily_->add(
          {}, Histogram::BucketBoundaries{1, 5, 10, 20, 40, 80, 160, 320, 640,
                                          1280, 2560})) {}

static std::string getAcceptedEncoding(struct mg_connection* conn) {
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

  auto startTimeOfRequest = std::chrono::steady_clock::now();

  auto acceptedEncoding = getAcceptedEncoding(conn);
  auto metrics = collectMetrics();

  auto contentType = std::string{};

  auto serializer = std::unique_ptr<Serializer>{};

  if (acceptedEncoding.find("application/vnd.google.protobuf") !=
      std::string::npos) {
    serializer.reset(new ProtobufDelimitedSerializer());
    contentType =
        "application/vnd.google.protobuf; "
        "proto=io.prometheus.client.MetricFamily; "
        "encoding=delimited";
  } else if (acceptedEncoding.find("application/json") != std::string::npos) {
    serializer.reset(new JsonSerializer());
    contentType = "application/json";
  } else {
    serializer.reset(new TextSerializer());
    contentType = "text/plain";
  }

  auto body = serializer->Serialize(metrics);
  mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n",
            contentType.c_str());
  mg_printf(conn, "Content-Length: %lu\r\n\r\n", body.size());
  mg_write(conn, body.data(), body.size());

  auto stopTimeOfRequest = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stopTimeOfRequest - startTimeOfRequest);
  requestLatencies_->observe(duration.count());

  bytesTransfered_->inc(body.size());
  numScrapes_->inc();
  return true;
}
std::vector<io::prometheus::client::MetricFamily>
MetricsHandler::collectMetrics() const {
  auto collectedMetrics = std::vector<io::prometheus::client::MetricFamily>{};

  for (auto&& wcollectable : collectables_) {
    auto collectable = wcollectable.lock();
    if (!collectable) {
      continue;
    }

    for (auto metric : collectable->collect()) {
      collectedMetrics.push_back(metric);
    }
  }

  return collectedMetrics;
}
}
}
