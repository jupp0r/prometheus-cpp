#include "prometheus/registry.h"

#include <gmock/gmock.h>

#include <cstddef>
#include <vector>

#include "mock_serializer.h"
#include "prometheus/counter.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/info.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace {

using ::testing::_;
using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::InSequence;
using ::testing::Sequence;

TEST(RegistryTest, collect_single_metric_family) {
  Registry registry{};
  auto& counter_family =
      BuildCounter().Name("test").Help("a test").Register(registry);
  counter_family.Add({{"name", "counter1"}});
  counter_family.Add({{"name", "counter2"}});

  MockSerializer serializer;

  {
    Sequence s1, s2;
    EXPECT_CALL(
        serializer,
        Serialize(AllOf(Field(&MetricFamily::name, "test"),
                        Field(&MetricFamily::help, "a test"),
                        Field(&MetricFamily::type, MetricType::Counter))))
        .InSequence(s1, s2);
    EXPECT_CALL(serializer,
                Serialize(_, AllOf(Field(&ClientMetric::label,
                                         ElementsAre(ClientMetric::Label{
                                             "name", "counter1"})))))
        .InSequence(s1);
    EXPECT_CALL(serializer,
                Serialize(_, AllOf(Field(&ClientMetric::label,
                                         ElementsAre(ClientMetric::Label{
                                             "name", "counter2"})))))
        .InSequence(s2);
  }

  registry.Collect(serializer);
}

TEST(RegistryTest, build_histogram_family) {
  Registry registry{};
  auto& histogram_family =
      BuildHistogram().Name("hist").Help("Test Histogram").Register(registry);
  auto& histogram = histogram_family.Add({{"name", "test_histogram_1"}},
                                         Histogram::BucketBoundaries{0, 1, 2});
  histogram.Observe(1.1);

  MockSerializer serializer;

  {
    InSequence seq;
    EXPECT_CALL(serializer, Serialize(_));
    EXPECT_CALL(serializer, Serialize(_, _)).Times(1);
  }

  registry.Collect(serializer);
}

TEST(RegistryTest, unable_to_remove_family) {
  Family<Counter> family{"name", "help", {}};
  Registry registry{};
  EXPECT_FALSE(registry.Remove(family));
}

TEST(RegistryTest, remove_and_readd_family) {
  Registry registry{Registry::InsertBehavior::Throw};

  auto& counter = BuildCounter().Name("name").Register(registry);
  EXPECT_TRUE(registry.Remove(counter));
  EXPECT_NO_THROW(BuildCounter().Name("name").Register(registry));
}

TEST(RegistryTest, reject_different_type_than_counter) {
  const auto same_name = std::string{"same_name"};
  Registry registry{};

  EXPECT_NO_THROW(BuildCounter().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildGauge().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildHistogram().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildInfo().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildSummary().Name(same_name).Register(registry));
}

TEST(RegistryTest, reject_different_type_than_gauge) {
  const auto same_name = std::string{"same_name"};
  Registry registry{};

  EXPECT_NO_THROW(BuildGauge().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildCounter().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildHistogram().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildInfo().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildSummary().Name(same_name).Register(registry));
}

TEST(RegistryTest, reject_different_type_than_histogram) {
  const auto same_name = std::string{"same_name"};
  Registry registry{};

  EXPECT_NO_THROW(BuildHistogram().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildInfo().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildCounter().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildGauge().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildSummary().Name(same_name).Register(registry));
}

TEST(RegistryTest, reject_different_type_than_info) {
  const auto same_name = std::string{"same_name"};
  Registry registry{};

  EXPECT_NO_THROW(BuildInfo().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildCounter().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildGauge().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildSummary().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildHistogram().Name(same_name).Register(registry));
}

TEST(RegistryTest, reject_different_type_than_summary) {
  const auto same_name = std::string{"same_name"};
  Registry registry{};

  EXPECT_NO_THROW(BuildSummary().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildCounter().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildGauge().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildHistogram().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildInfo().Name(same_name).Register(registry));
}

TEST(RegistryTest, throw_for_same_family_name) {
  const auto same_name = std::string{"same_name"};
  Registry registry{Registry::InsertBehavior::Throw};

  EXPECT_NO_THROW(BuildCounter().Name(same_name).Register(registry));
  EXPECT_ANY_THROW(BuildCounter().Name(same_name).Register(registry));
}

TEST(RegistryTest, merge_same_families) {
  Registry registry{Registry::InsertBehavior::Merge};

  std::size_t loops = 4;

  while (loops-- > 0) {
    BuildCounter()
        .Name("counter")
        .Help("Test Counter")
        .Register(registry)
        .Add({{"name", "test_counter"}});
  }

  MockSerializer serializer;

  {
    InSequence seq;
    EXPECT_CALL(serializer, Serialize(_));
    EXPECT_CALL(serializer, Serialize(_, _)).Times(1);
  }

  registry.Collect(serializer);
}

TEST(RegistryTest, do_not_merge_families_with_different_labels) {
  Registry registry{Registry::InsertBehavior::Merge};

  EXPECT_NO_THROW(BuildCounter()
                      .Name("counter")
                      .Help("Test Counter")
                      .Labels({{"a", "A"}})
                      .Register(registry));

  EXPECT_ANY_THROW(BuildCounter()
                       .Name("counter")
                       .Help("Test Counter")
                       .Labels({{"b", "B"}})
                       .Register(registry));
}

}  // namespace
}  // namespace prometheus
