#include "counter.h"

namespace prometheus {

void Counter::inc() { gauge_.inc(); }

void Counter::inc(double val) { gauge_.inc(val); }

double Counter::value() const { return gauge_.value(); }
}
