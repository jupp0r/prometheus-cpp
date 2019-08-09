#pragma once

#include <map>
#include <string>

#include "prometheus/registry.h"

namespace prometheus {

template <typename T>
class Family;

namespace detail {

template <typename T>
class Builder {
 public:
  Builder& Labels(const std::map<std::string, std::string>& labels);
  Builder& Name(const std::string&);
  Builder& Help(const std::string&);
  Family<T>& Register(Registry&);

 private:
  std::map<std::string, std::string> labels_;
  std::string name_;
  std::string help_;
};

template <typename T>
Builder<T>& Builder<T>::Labels(
   const std::map<std::string, std::string>& labels) {
 labels_ = labels;
 return *this;
}

template <typename T>
Builder<T>& Builder<T>::Name(const std::string& name) {
 name_ = name;
 return *this;
}

template <typename T>
Builder<T>& Builder<T>::Help(const std::string& help) {
 help_ = help;
 return *this;
}

template <typename T>
Family<T>& Builder<T>::Register(Registry& registry) {
 return registry.Add<T>(name_, help_, labels_);
}

}  // namespace detail
}  // namespace prometheus