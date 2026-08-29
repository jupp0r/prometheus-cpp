#pragma once

#include <functional>
#include <map>
#include <string>

namespace prometheus {

/// \brief Multiple labels and their value.
///
/// Uses the transparent comparator std::less<> to allow heterogeneous lookup
using Labels = std::map<std::string, std::string, std::less<>>;

/// \brief Single label and its value.
using Label = Labels::value_type;

}  // namespace prometheus
