#include "text_serializer.h"
#include <sstream>

namespace prometheus {

std::string TextSerializer::Serialize(
    const std::vector<io::prometheus::client::MetricFamily>& metrics) {
  auto result = std::string{};
  for (auto&& metric : metrics) {
	result += TextSerializer::CovertToString(metric) + "\n";
  }
  return result;
}

std::string TextSerializer::LabelStr(
		io::prometheus::client::Metric& m) {
	std::string label_str = "";
	int label_size = m.label_size();
	for (int j=0; j<label_size; j++) {
		auto label_pair = m.label(j);
		if (j == label_size - 1) {
			label_str = label_str + label_pair.name() + "=\"" + label_pair.value() + "\"";
		} else {
			label_str = label_str + label_pair.name() + "=\"" + label_pair.value() + "\",";
		}
	}
	return label_str;
}

std::string TextSerializer::SingleMetricStr(io::prometheus::client::Metric& m,
		const std::string& metric_name, 
		const io::prometheus::client::MetricType type) {
  using namespace io::prometheus::client;
  std::string label_str = LabelStr(m);
  int label_size = m.label_size();
  std::string ts = m.has_timestamp_ms() ? std::to_string(m.timestamp_ms()) : "";

  if (type == MetricType::HISTOGRAM) {
	std::string res = "";
	int bsize = m.histogram().bucket_size();
	for (int k=0; k<bsize; k++) {
		auto bucket = m.histogram().bucket(k);
		std::string line = metric_name + "_bucket{le=\"" + 
			std::to_string(bucket.upper_bound()) + "\"," + label_str + "} " + 
			std::to_string(bucket.cumulative_count()) + " " + ts + "\n";
		res += line;
	}
	res += metric_name + "_sum{" + label_str + "} " + 
		std::to_string(m.histogram().sample_sum()) + "\n";
	res += metric_name + "_count{" + label_str + "} " + 
		std::to_string(m.histogram().sample_count()) + "\n";
	return res;
  }else if (type == MetricType::SUMMARY) {
	return "Not supported by now";
  }else {
	  std::string value = std::to_string(m.counter().value());
	if (label_size != 0) {
		return (metric_name + "{" + LabelStr(m) + "} " + value + " " + ts + "\n");
	} else {
		return (metric_name + " " + value + " " + ts + "\n");
	}
  }
}

std::string TextSerializer::CovertToString(
		const io::prometheus::client::MetricFamily& metric_family) {
  using namespace io::prometheus::client;
  auto result = std::string{};
  auto repeated_field_metric = metric_family.metric();
  int size = repeated_field_metric.size();

  std::stringstream ss;
  auto metric_type = metric_family.type();
  std::string metric_family_name = metric_family.name();
  if (metric_family.has_help()) {
	ss << "# HELP " << metric_family_name << " " << metric_family.help() << std::endl;
  }
  if (metric_family.has_type()) {
	ss << "# TYPE " << metric_family_name << " " << 
		MetricType_Name(metric_type) << std::endl;
  }

  for (int i=0; i<size; i++) {
	auto m = repeated_field_metric.Get(i);
	ss << TextSerializer::SingleMetricStr(m,metric_family_name,metric_type);
  }

  return ss.str();
}

}
