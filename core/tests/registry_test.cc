#include <vector>

#include <gmock/gmock.h>

#include <prometheus/collectable.h>
#include <prometheus/registry.h>

using namespace testing;
using namespace prometheus;

TEST(RegistryTest, collect_single_metric_family) {
  Registry registry{};
  auto& counter_family =
      BuildCounter().Name("test").Help("a test").Register(registry);
  counter_family.Add({{"name", "counter1"}});
  counter_family.Add({{"name", "counter2"}});
  auto collected = registry.Collect();
  ASSERT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].name, "test");
  EXPECT_EQ(collected[0].help, "a test");
  ASSERT_EQ(collected[0].metric.size(), 2);
  ASSERT_EQ(collected[0].metric.at(0).label.size(), 1);
  EXPECT_EQ(collected[0].metric.at(0).label.at(0).name, "name");
  ASSERT_EQ(collected[0].metric.at(1).label.size(), 1);
  EXPECT_EQ(collected[0].metric.at(1).label.at(0).name, "name");
}

TEST(RegistryTest, build_histogram_family) {
  Registry registry{};
  auto& histogram_family =
      BuildHistogram().Name("hist").Help("Test Histogram").Register(registry);
  auto& histogram = histogram_family.Add({{"name", "test_histogram_1"}},
                                         Histogram::BucketBoundaries{0, 1, 2});
  histogram.Observe(1.1);
  auto collected = registry.Collect();
  ASSERT_EQ(collected.size(), 1);
}
