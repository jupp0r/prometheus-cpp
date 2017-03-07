#pragma once

#include <map>
#include <string>
#include <vector>

namespace prometheus {

template <typename T>
class Family;
class Histogram;
class Registry;

namespace detail {
class HistogramBuilder;
}

detail::HistogramBuilder BuildHistogram();

namespace detail {
class HistogramBuilder {
 public:
  HistogramBuilder& Labels(const std::map<std::string, std::string>& labels);
  HistogramBuilder& Name(const std::string&);
  HistogramBuilder& Help(const std::string&);
  HistogramBuilder& Buckets(const std::vector<double>&);
  Family<Histogram>& Register(Registry&);

 private:
  std::map<std::string, std::string> labels_;
  std::string name_;
  std::string help_;
  std::vector<double> buckets_;
};
}
}
