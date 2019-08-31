#pragma once

#include "prometheus/registry.h"

namespace prometheus {

template <typename T>
Family<T>& Registry::Add(const std::string& name, const std::string& help,
                         const std::map<std::string, std::string>& labels) {
  std::lock_guard<std::mutex> lock{mutex_};
  auto family = detail::make_unique<Family<T>>(name, help, labels);
  auto& ref = *family;
  collectables_.push_back(std::move(family));
  return ref;
}

}  // namespace prometheus
