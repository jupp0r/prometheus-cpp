#include "prometheus/counter.h"

namespace prometheus {

void Counter::Increment() { gauge_.Increment(); }

void Counter::Increment(double val) { gauge_.Increment(val); }

double Counter::Value() const { return gauge_.Value(); }

void Counter::Set(double value) { return gauge_.Set(value); }

io::prometheus::client::Metric Counter::Collect() {
  io::prometheus::client::Metric metric;
  auto counter = metric.mutable_counter();
  counter->set_value(Value());
  return metric;
}
}
