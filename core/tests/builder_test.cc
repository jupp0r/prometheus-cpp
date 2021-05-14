#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/family.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/registry.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace {

class BuilderTest : public testing::Test {
 protected:
  enum class Variable { no, yes };

  template <Variable v>
  std::vector<ClientMetric::Label> getExpectedLabels() {
    std::vector<ClientMetric::Label> labels;

    int i = 0;
    auto gen_pair = [](std::pair<const std::string, std::string> p) {
      return ClientMetric::Label{p.first, p.second};
    };
    auto gen_vec = [&](const std::string k) {
      return ClientMetric::Label{k, variable_values[i++]};
    };


    std::transform(std::begin(const_labels), std::end(const_labels),
                   std::back_inserter(labels), gen_pair);
    if ( v == Variable::no ) {
      std::transform(std::begin(more_labels), std::end(more_labels),
                     std::back_inserter(labels), gen_pair);
    }else {
      std::transform(std::begin(variable_labels), std::end(variable_labels),
                     std::back_inserter(labels), gen_vec);

    }

    return labels;
  }

  template <Variable v>
  void verifyCollectedLabels() {
    const auto collected = registry.Collect();

    ASSERT_EQ(1U, collected.size());
    EXPECT_EQ(name, collected.at(0).name);
    EXPECT_EQ(help, collected.at(0).help);
    ASSERT_EQ(1U, collected.at(0).metric.size());

    //    std::cout<<"===========got==========="<<std::endl;
//    auto& l = collected.at(0).metric.at(0).label;
//    std::for_each(l.begin(),l.end(),
//                  [](const ClientMetric::Label& label){
//                    std::cout<<label.name<<":"<<label.value<<std::endl;
//                  });
//    std::cout<<"===========end==========="<<std::endl;
    if ( v == Variable::no) {
//      std::cout<<"===========expected==========="<<std::endl;
//      std::for_each(expected_labels.begin(),expected_variable_labels.end(),
//                    [](const ClientMetric::Label& label){
//                      std::cout<<label.name<<":"<<label.value<<std::endl;
//                    });
//      std::cout<<"=============end============="<<std::endl;
      EXPECT_THAT(collected.at(0).metric.at(0).label,
                  testing::UnorderedElementsAreArray(expected_labels));
    } else {
//      std::cout<<"===========expected==========="<<std::endl;
//      std::for_each(expected_variable_labels.begin(),expected_variable_labels.end(),
//                    [](const ClientMetric::Label& label){
//                      std::cout<<label.name<<":"<<label.value<<std::endl;
//                    });
//      std::cout<<"=============end============="<<std::endl;

      EXPECT_THAT(collected.at(0).metric.at(0).label,
                  testing::UnorderedElementsAreArray(expected_variable_labels));
    }
  }

  Registry registry;

  const std::string name = "some_name";
  const std::string help = "Additional description.";
  const std::vector<std::string> variable_labels = {"vkey","vname"};
  const std::vector<std::string> variable_values = {"vvalue","vtest"};
  const std::map<std::string, std::string> const_labels = {{"key", "value"}};
  const std::map<std::string, std::string> more_labels = {{"name", "test"}};
  const std::vector<ClientMetric::Label> expected_labels = getExpectedLabels<Variable::no>();
  const std::vector<ClientMetric::Label> expected_variable_labels = getExpectedLabels<Variable::yes>();
};

TEST_F(BuilderTest, build_counter) {
  auto& family = BuildCounter()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(registry);
  family.Add(more_labels);

  verifyCollectedLabels<Variable::no>();
}

TEST_F(BuilderTest, build_gauge) {
  auto& family = BuildGauge()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(registry);
  family.Add(more_labels);

  verifyCollectedLabels<Variable::no>();
}

TEST_F(BuilderTest, build_histogram) {
  auto& family = BuildHistogram()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(registry);
  family.Add(more_labels, Histogram::BucketBoundaries{1, 2});

  verifyCollectedLabels<Variable::no>();
}

TEST_F(BuilderTest, build_histogram_vec) {
  auto& family = BuildHistogram()
          .Name(name)
          .Help(help)
          .Labels(const_labels)
          .LabelNamesVec(variable_labels)
          .BucketBoundaries({Histogram::BucketBoundaries{1, 2}})
          .Register(registry);
  family.WithLabelValues(variable_values);

  verifyCollectedLabels<Variable::yes>();
}

TEST_F(BuilderTest, build_summary) {
  auto& family = BuildSummary()
                     .Name(name)
                     .Help(help)
                     .Labels(const_labels)
                     .Register(registry);
  family.Add(more_labels, Summary::Quantiles{});

  verifyCollectedLabels<Variable::no>();
}

TEST_F(BuilderTest, build_summary_vec) {
  auto& family = BuildSummary()
          .Name(name)
          .Help(help)
          .Labels(const_labels)
          .LabelNamesVec(variable_labels)
          .Quantiles(Summary::Quantiles{})
          .Register(registry);
  family.WithLabelValues(variable_values);

  verifyCollectedLabels<Variable::yes>();
}

}  // namespace
}  // namespace prometheus
