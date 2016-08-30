#include "Counter.h"

namespace prometheus {

Counter::Counter()
        : value_{0}
{}

void Counter::increment(double val) {
  auto current = value_.load();
  while (!value_.compare_exchange_weak(current, current + val))
    ;
}
}
