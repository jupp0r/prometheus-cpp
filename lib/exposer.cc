#include <chrono>
#include <sstream>
#include <string>
#include <thread>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

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

bool MetricsHandler::handleGet(CivetServer* server,
                               struct mg_connection* conn) {
  using namespace io::prometheus::client;

  std::ostringstream ss;
  for (auto&& wcollectable : collectables_) {
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

  auto body = ss.str();
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
