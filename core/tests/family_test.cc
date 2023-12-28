#include "prometheus/family.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "mock_serializer.h"
#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/detail/future_std.h"
#include "prometheus/histogram.h"
#include "prometheus/labels.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace {

using ::testing::_;
using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::InSequence;
using ::testing::IsEmpty;
using ::testing::Sequence;

TEST(FamilyTest, labels) {
  const auto const_labels = Labels{{"component", "test"}};
  const auto dynamic_labels = Labels{{"status", "200"}};

  Family<Counter> family{"total_requests", "Counts all requests", const_labels};
  family.Add(dynamic_labels);

  MockSerializer serializer;

  {
    InSequence seq;
    EXPECT_CALL(serializer, Serialize(_));
    EXPECT_CALL(serializer, Serialize(_, const_labels, dynamic_labels, _))
        .Times(1);
  }

  family.Collect(serializer);
}

TEST(FamilyTest, reject_same_label_keys) {
  auto labels = Labels{{"component", "test"}};

  Family<Counter> family{"total_requests", "Counts all requests", labels};
  EXPECT_ANY_THROW(family.Add(labels));
}

TEST(FamilyTest, counter_value) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  auto& counter = family.Add({});
  counter.Increment();

  MockSerializer serializer;

  {
    InSequence seq;
    EXPECT_CALL(serializer, Serialize(_));
    EXPECT_CALL(serializer,
                Serialize(Field(&MetricFamily::type, MetricType::Counter),
                          IsEmpty(), IsEmpty(),
                          Field(&ClientMetric::counter,
                                Field(&ClientMetric::Counter::value, 1))))
        .Times(1);
  }

  family.Collect(serializer);
}

TEST(FamilyTest, remove) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  auto& counter1 = family.Add({{"name", "counter1"}});
  family.Add({{"name", "counter2"}});
  family.Remove(&counter1);

  MockSerializer serializer;

  {
    InSequence seq;
    EXPECT_CALL(serializer, Serialize(_));
    EXPECT_CALL(serializer, Serialize(_, _, _, _)).Times(1);
  }

  family.Collect(serializer);
}

TEST(FamilyTest, removeUnknownMetricMustNotCrash) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  family.Remove(nullptr);
}

TEST(FamilyTest, Histogram) {
  Family<Histogram> family{"request_latency", "Latency Histogram", {}};
  auto& histogram1 = family.Add({{"name", "histogram1"}},
                                Histogram::BucketBoundaries{0, 1, 2});
  histogram1.Observe(0);

  MockSerializer serializer;

  {
    InSequence seq;
    EXPECT_CALL(serializer, Serialize(_));
    EXPECT_CALL(
        serializer,
        Serialize(Field(&MetricFamily::type, MetricType::Histogram), _, _,
                  Field(&ClientMetric::histogram,
                        Field(&ClientMetric::Histogram::sample_count, 1))))
        .Times(1);
  }

  family.Collect(serializer);
}

TEST(FamilyTest, add_twice) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  auto& counter = family.Add({{"name", "counter1"}});
  auto& counter1 = family.Add({{"name", "counter1"}});
  ASSERT_EQ(&counter, &counter1);
}

TEST(FamilyTest, throw_on_invalid_metric_name) {
  auto create_family_with_invalid_name = []() {
    return detail::make_unique<Family<Counter>>("", "empty name", Labels{});
  };
  EXPECT_ANY_THROW(create_family_with_invalid_name());
}

TEST(FamilyTest, throw_on_invalid_constant_label_name) {
  auto create_family_with_invalid_labels = []() {
    return detail::make_unique<Family<Counter>>(
        "total_requests", "Counts all requests",
        Labels{{"__inavlid", "counter1"}});
  };
  EXPECT_ANY_THROW(create_family_with_invalid_labels());
}

TEST(FamilyTest, should_throw_on_invalid_labels) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  auto add_metric_with_invalid_label_name = [&family]() {
    family.Add({{"__invalid", "counter1"}});
  };
  EXPECT_ANY_THROW(add_metric_with_invalid_label_name());
}

TEST(FamilyTest, should_not_collect_empty_metrics) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  MockSerializer serializer;
  family.Collect(serializer);
}

TEST(FamilyTest, query_family_if_metric_already_exists) {
  Family<Counter> family{"total_requests", "Counts all requests", {}};
  family.Add({{"name", "counter1"}});
  EXPECT_TRUE(family.Has({{"name", "counter1"}}));
  EXPECT_FALSE(family.Has({{"name", "counter2"}}));
}

TEST(FamilyTest, reject_histogram_with_constant_le_label) {
  auto labels = Labels{{"le", "test"}};
  EXPECT_ANY_THROW(
      detail::make_unique<Family<Histogram>>("name", "help", labels));
}

TEST(FamilyTest, reject_histogram_with_le_label) {
  Family<Histogram> family{"name", "help", {}};
  auto labels = Labels{{"le", "test"}};
  EXPECT_ANY_THROW(family.Add(labels, Histogram::BucketBoundaries{0, 1, 2}));
}

TEST(FamilyTest, reject_summary_with_constant_quantile_label) {
  auto labels = Labels{{"quantile", "test"}};
  EXPECT_ANY_THROW(
      detail::make_unique<Family<Summary>>("name", "help", labels));
}

TEST(FamilyTest, reject_summary_with_quantile_label) {
  Family<Summary> family{"name", "help", {}};
  auto labels = Labels{{"quantile", "test"}};
  auto quantiles = Summary::Quantiles{{0.5, 0.05}};
  EXPECT_ANY_THROW(family.Add(labels, quantiles));
}

}  // namespace
}  // namespace prometheus
