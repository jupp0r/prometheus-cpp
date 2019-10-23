#pragma once

#include <map>
#include <vector>
#include <string>
#include "prometheus/detail/ckms_quantiles.h"

namespace prometheus {

class Counter;
class Summary;
class Histogram;
template <typename T>
class Family;
class Registry;

namespace detail {

template <typename T>
class Builder {
 public:
  Builder& Labels(const std::map<std::string, std::string>& labels);
  Builder& LabelNamesVec(const std::vector<std::string>& labels);
  Builder& Name(const std::string&);
  Builder& Help(const std::string&);
  Family<T>& Register(Registry&);

  template <typename U= T, typename = typename std::enable_if<std::is_same<U, Summary>::value,Summary>::type>
  Builder& Quantiles(const std::vector<detail::CKMSQuantiles::Quantile>&);

  template <typename U= T, typename = typename std::enable_if<std::is_same<U, Histogram>::value,Histogram>::type>
  Builder& BucketBoundaries(const std::vector<double>&);

private:
  std::map<std::string, std::string> labels_;
  std::vector<std::string> variable_labels_;
  std::vector<detail::CKMSQuantiles::Quantile> quantiles_;
  std::vector<double> bucket_boundaries_;
  std::string name_;
  std::string help_;
};

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
