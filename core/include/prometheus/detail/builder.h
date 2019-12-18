#pragma once

#include <map>
#include <string>
#include <memory>

#include "prometheus/family.h"

namespace prometheus {

class Registry;

namespace detail {

template <typename T>
class Builder {
 public:
  Builder& RetentionBehavior(const prometheus::RetentionBehavior& retention_behavior);
  Builder& Labels(const std::map<std::string, std::string>& labels);
  Builder& Name(const std::string&);
  Builder& Help(const std::string&);
  std::shared_ptr<Family<T>> Register(Registry&);

 private:
  prometheus::RetentionBehavior retention_behavior_ = prometheus::RetentionBehavior::Keep;
  std::map<std::string, std::string> labels_;
  std::string name_;
  std::string help_;
};

}  // namespace detail
}  // namespace prometheus
