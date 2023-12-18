#pragma once

#include <iosfwd>

#include "prometheus/labels.h"

namespace prometheus {
namespace detail {

void encodeLabel(std::ostream& os, const Label& label);

}
}  // namespace prometheus