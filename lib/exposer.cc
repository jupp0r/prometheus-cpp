#include <chrono>
#include <string>
#include <thread>
#include <sstream>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "exposer.h"

#include "cpp/metrics.pb.h"

namespace prometheus {

class MetricsHandler : public CivetHandler {
 public:
  bool handleGet(CivetServer* server, struct mg_connection* conn) {
      using namespace io::prometheus::client;

    MetricFamily message;
    message.set_name("Foo");
    message.set_help("Foo help");
    message.set_type(MetricType::COUNTER);
    auto metric1 = message.add_metric();
    auto counter = metric1->mutable_counter();
    counter->set_value(1337.0);

    std::ostringstream ss;
    {
        google::protobuf::io::OstreamOutputStream rawOutput{&ss};
        google::protobuf::io::CodedOutputStream output(&rawOutput);

        // Write the size.
        const int size = message.ByteSize();
        output.WriteVarint32(size);
    }

    auto buf = ss.str();
    message.AppendToString(&buf);
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: "
              "application/vnd.google.protobuf; "
              "proto=io.prometheus.client.MetricFamily; "
              "encoding=delimited\r\n"
              "Content-Length: ");
    mg_printf(conn, "%lu\r\n\r\n", buf.size());
    mg_write(conn, buf.data(), buf.size());
    return true;
  }
};

Exposer::Exposer(std::uint16_t port)
    : server_({"listening_ports", std::to_string(port)}) {
  MetricsHandler handler;
  server_.addHandler("/metrics", &handler);
  std::this_thread::sleep_for(std::chrono::seconds(60000));
}

void Exposer::run() {}
}
