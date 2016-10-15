#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/util/json_util.h>

#include "exposer.h"

#include "cpp/metrics.pb.h"

namespace prometheus {
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
      numScrapes_(numScrapesFamily_->add({})) {}

static std::string serializeToDelimitedProtobuf(
    const std::vector<std::weak_ptr<Collectable>>& collectables) {
  std::ostringstream ss;
  for (auto&& wcollectable : collectables) {
    auto collectable = wcollectable.lock();
    if (!collectable) {
      continue;
    }

    for (auto&& metricFamily : collectable->collect()) {
      {
        google::protobuf::io::OstreamOutputStream rawOutput{&ss};
        google::protobuf::io::CodedOutputStream output(&rawOutput);

        const int size = metricFamily.ByteSize();
        output.WriteVarint32(size);
      }

      auto buffer = std::string{};
      metricFamily.SerializeToString(&buffer);
      ss << buffer;
    }
  }
  return ss.str();
}

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

  auto acceptedEncoding = getAcceptedEncoding(conn);
  if (acceptedEncoding.find("application/vnd.google.protobuf") !=
      std::string::npos) {
    auto body = serializeToDelimitedProtobuf(collectables_);
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: "
              "application/vnd.google.protobuf; "
              "proto=io.prometheus.client.MetricFamily; "
              "encoding=delimited\r\n"
              "Content-Length: ");
    mg_printf(conn, "%lu\r\n\r\n", body.size());
    mg_write(conn, body.data(), body.size());
    bytesTransfered_->inc(body.size());
  } else if (acceptedEncoding.find("application/json") != std::string::npos) {
    std::stringstream ss;
    ss << "[";

    for (auto&& wcollectable : collectables_) {
      auto collectable = wcollectable.lock();
      if (!collectable) {
        continue;
      }

      for (auto&& metricFamily : collectable->collect()) {
        std::string result;
        google::protobuf::util::MessageToJsonString(
            metricFamily, &result, google::protobuf::util::JsonPrintOptions());
        ss << result;
        if (collectable != collectables_.back().lock()) {
          ss << ",";
        }
      }
    }
    ss << "]";
    auto body = ss.str();
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: application/json\r\n"
              "Content-Length: ");
    mg_printf(conn, "%lu\r\n\r\n", body.size());
    mg_write(conn, body.data(), body.size());
    bytesTransfered_->inc(body.size());
  } else {
    auto body = std::string{};
    for (auto&& wcollectable : collectables_) {
      auto collectable = wcollectable.lock();
      if (!collectable) {
        continue;
      }

      for (auto&& metricFamily : collectable->collect()) {
        body += metricFamily.DebugString() + "\n";
      }
      mg_printf(conn,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: ");
      mg_printf(conn, "%lu\r\n\r\n", body.size());
      mg_write(conn, body.data(), body.size());
      bytesTransfered_->inc(body.size());
    }
  }

  numScrapes_->inc();
  return true;
}

Exposer::Exposer(std::uint16_t port)
    : server_({"listening_ports", std::to_string(port)}),
      exposerRegistry_(
          std::make_shared<Registry>(std::map<std::string, std::string>{})),
      metricsHandler_(collectables_, *exposerRegistry_) {
  registerCollectable(exposerRegistry_);
  server_.addHandler("/metrics", &metricsHandler_);
}

void Exposer::registerCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  collectables_.push_back(collectable);
}
}
