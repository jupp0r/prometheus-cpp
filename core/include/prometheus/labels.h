#pragma once

#if defined(__cpp_lib_generic_associative_lookup)
#include <functional>
#endif
#include <map>
#include <string>

namespace prometheus {

/// \brief Multiple labels and their value.
///
/// Uses the transparent comparator std::less<> on C++14 and newer to allow
/// heterogeneous lookup, falling back to the default comparator on C++11.
#if defined(__cpp_lib_generic_associative_lookup)
using Labels = std::map<std::string, std::string, std::less<>>;
#else
using Labels = std::map<std::string, std::string>;
#endif

/// \brief Single label and its value.
using Label = Labels::value_type;

}  // namespace prometheus
