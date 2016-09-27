#include "counter.h"
#include "cpp/metrics.pb.h"

namespace prometheus {

void Counter::inc() { gauge_.inc(); }

void Counter::inc(double val) { gauge_.inc(val); }

double Counter::value() const { return gauge_.value(); }

io::prometheus::client::Metric Counter::collect() {
  io::prometheus::client::Metric metric;
  auto counter = metric.mutable_counter();
  counter->set_value(value());
  return metric;
}
}
