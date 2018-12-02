#pragma onece

#include <map>
#include <cstddef>
#include <string>

namespace prometheus {

namespace utils {

///TODO(qwang): doc and test this.
std::size_t hash_labels(const std::map<std::string, std::string>& labels);

}  // utils

}  // prometheus
