#pragma once

#include "prometheus/detail/builder.h"

namespace prometheus {

namespace detail {

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
