#include <iostream>
#include <sstream>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "protobuf_delimited_serializer.h"

namespace prometheus {

std::string ProtobufDelimitedSerializer::Serialize(
    const std::vector<io::prometheus::client::MetricFamily>& metrics) {
  std::ostringstream ss;
  for (auto&& metric : metrics) {
    {
      google::protobuf::io::OstreamOutputStream raw_output{&ss};
      google::protobuf::io::CodedOutputStream output(&raw_output);

      const int size = metric.ByteSize();
      output.WriteVarint32(size);
    }

    auto buffer = std::string{};
    metric.SerializeToString(&buffer);
    ss << buffer;
  }
  return ss.str();
}
}
