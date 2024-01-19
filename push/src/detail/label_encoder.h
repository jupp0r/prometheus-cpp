#pragma once

#include <iosfwd>

#include "prometheus/labels.h"

namespace prometheus {
namespace detail {

#if defined _WIN32
  #ifdef __GNUC__
    void encodeLabel(std::ostream& os, const Label& label) __attribute__ ((dllexport))
  #else
    void encodeLabel(std::ostream& os, const Label& label) __declspec(dllexport)
  #endif
#else
  void encodeLabel(std::ostream& os, const Label& label) __attribute__ ((visibility ("default")));
#endif

}
}  // namespace prometheus