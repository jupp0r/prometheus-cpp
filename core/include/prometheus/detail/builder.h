#pragma once

#include <map>
#include <string>
#include <memory>

namespace prometheus {

template <typename T>
class Family;
class Registry;

namespace detail {

template <typename T>
class Builder {
 public:
  Builder& Labels(const std::map<std::string, std::string>& labels);
  Builder& Name(const std::string&);
  Builder& Help(const std::string&);
  std::shared_ptr<Family<T>> Register(Registry&);

 private:
  std::map<std::string, std::string> labels_;
  std::string name_;
  std::string help_;
};

}  // namespace detail
}  // namespace prometheus
