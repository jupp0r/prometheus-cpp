#include <memory>

#include <gmock/gmock.h>

#include <prometheus/client_metric.h>
#include <prometheus/exposer.h>
#include <prometheus/family.h>
#include <prometheus/histogram.h>

using namespace testing;
using namespace prometheus;

class FamilyTest : public Test {};

TEST_F(FamilyTest, labels) {
  auto const_label = ClientMetric::Label{"component", "test"};
  auto dynamic_label = ClientMetric::Label{"status", "200"};

  Family<Counter> family{"total_requests",
                         "Counts all requests",
                         {{const_label.name, const_label.value}}};
  family.Add({{dynamic_label.name, dynamic_label.value}});
  auto collected = family.Collect();
  ASSERT_GE(collected.size(), 1);
  ASSERT_GE(collected.at(0).metric.size(), 1);
  EXPECT_THAT(collected.at(0).metric.at(0).label,
              ElementsAre(const_label, dynamic_label));
}

TEST_F(FamilyTest, counter_value) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  auto& counter = family.Add({});
  counter.Increment();
  auto collected = family.Collect();
  ASSERT_GE(collected.size(), 1);
  ASSERT_GE(collected[0].metric.size(), 1);
  EXPECT_THAT(collected[0].metric.at(0).counter.value, Eq(1));
}

TEST_F(FamilyTest, remove) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  auto& counter1 = family.Add({{"name", "counter1"}});
  family.Add({{"name", "counter2"}});
  family.Remove(&counter1);
  auto collected = family.Collect();
  ASSERT_GE(collected.size(), 1);
  EXPECT_EQ(collected[0].metric.size(), 1);
}

TEST_F(FamilyTest, Histogram) {
  Family<Histogram> family{"request_latency", "Latency Histogram", {}};
  auto& histogram1 = family.Add({{"name", "histogram1"}},
                                Histogram::BucketBoundaries{0, 1, 2});
  histogram1.Observe(0);
  auto collected = family.Collect();
  ASSERT_EQ(collected.size(), 1);
  ASSERT_GE(collected[0].metric.size(), 1);
  EXPECT_THAT(collected[0].metric.at(0).histogram.sample_count, Eq(1));
}

TEST_F(FamilyTest, add_twice) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  auto& counter = family.Add({{"name", "counter1"}});
  auto& counter1 = family.Add({{"name", "counter1"}});
  ASSERT_EQ(&counter, &counter1);
}

#ifndef NDEBUG
TEST_F(FamilyTest, should_assert_on_invalid_metric_name) {
  auto create_family_with_invalid_name = []() {
    new Family<Counter>("", "empty name", {});
  };
  EXPECT_DEATH(create_family_with_invalid_name(), ".*");
}

TEST_F(FamilyTest, should_assert_on_invalid_labels) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  auto add_metric_with_invalid_label_name = [&family]() {
    family.Add({{"__invalid", "counter1"}});
  };
  EXPECT_DEATH(add_metric_with_invalid_label_name(), ".*");
}
#endif
