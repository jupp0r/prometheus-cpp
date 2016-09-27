#include "label_decorator.h"

#include "cpp/metrics.pb.h"
#include "google/protobuf/repeated_field.h"

namespace prometheus {

LabelDecorator::LabelDecorator(
    std::vector<std::pair<std::string, std::string>> labels,
    std::unique_ptr<Metric> metric)
    : labels_(std::move(labels)), metric_(std::move(metric)) {}

io::prometheus::client::Metric LabelDecorator::collect() {
  auto undecoratedMetric = metric_->collect();
  for (auto&& labelPair : labels_) {
    auto newLabelPair = undecoratedMetric.add_label();
    newLabelPair->set_name(labelPair.first);
    newLabelPair->set_value(labelPair.second);
  }
  return undecoratedMetric;
}
}
