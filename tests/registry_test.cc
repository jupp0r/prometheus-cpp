#include <vector>

#include <gmock/gmock.h>

#include "lib/collectable.h"
#include "lib/registry.h"

using namespace testing;
using namespace prometheus;

class MockCollectable : public Collectable {
 public:
  MOCK_METHOD0(Collect, std::vector<io::prometheus::client::MetricFamily>());
};

class RegistryTest : public Test {};

TEST_F(RegistryTest, collect_single_metric_family) {
  Registry registry{};
  auto& counter_family =
      BuildCounter().Name("test").Help("a test").Register(registry);
  counter_family.Add({{"name", "counter1"}});
  counter_family.Add({{"name", "counter2"}});
  auto collected = registry.Collect();
  ASSERT_EQ(collected.size(), 1);
  EXPECT_EQ(collected[0].name(), "test");
  EXPECT_EQ(collected[0].help(), "a test");
  ASSERT_EQ(collected[0].metric_size(), 2);
  ASSERT_EQ(collected[0].metric(0).label_size(), 1);
  EXPECT_EQ(collected[0].metric(0).label(0).name(), "name");
  ASSERT_EQ(collected[0].metric(1).label_size(), 1);
  EXPECT_EQ(collected[0].metric(1).label(0).name(), "name");
}
