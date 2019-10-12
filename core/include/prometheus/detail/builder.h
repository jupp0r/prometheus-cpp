#pragma once

#include <map>
#include <string>
#include <vector>

#include "prometheus/detail/ckms_quantiles.h"
#include "prometheus/registry.h"

namespace prometheus {

template <typename T>
class Family;
class Guage;
class Counter;
class Summary;
class Histogram;

namespace detail {

template <typename T>
class Builder {
 public:
  Builder& Labels(const std::map<std::string, std::string>& labels);
  Builder& LabelsVec(const std::vector<std::string>& labels);
  Builder& Name(const std::string&);
  Builder& Help(const std::string&);

  template <typename U= T, typename = typename std::enable_if<std::is_same<U, Summary>::value,Summary>::type>
  Builder& Quantiles(const std::vector<detail::CKMSQuantiles::Quantile>&);

  template <typename U= T, typename = typename std::enable_if<std::is_same<U, Histogram>::value,Histogram>::type>
  Builder& BucketBoundaries(const std::vector<double>&);

  Family<T>& Register(Registry&);

 private:
  std::map<std::string, std::string> labels_;
  std::vector<std::string> variable_labels_;
  std::vector<detail::CKMSQuantiles::Quantile> quantiles_;
  std::vector<double> bucket_boundaries_;

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
Builder<T>& Builder<T>::LabelsVec(
        const std::vector<std::string>& labels) {
  variable_labels_ = labels;
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
 return registry.Add<T>(name_, help_, variable_labels_, labels_);
}

template<typename T>
template<typename U, typename>
Builder<T>& Builder<T>::Quantiles(const std::vector<detail::CKMSQuantiles::Quantile>& quantiles) {
  quantiles_ = quantiles;
  return *this;
}

template<typename T>
template<typename U, typename>
Builder<T>& Builder<T>::BucketBoundaries(const std::vector<double>& bucket_boundaries) {
  bucket_boundaries_ = bucket_boundaries;
  return *this;
}

}  // namespace detail
}  // namespace prometheus