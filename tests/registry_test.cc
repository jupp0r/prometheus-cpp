#include <vector>

#include <gmock/gmock.h>

#include "lib/registry.h"
#include "lib/collectable.h"

using namespace testing;
using namespace prometheus;

class MockCollectable : public Collectable {
  public:
    MOCK_METHOD0(collect, std::vector<io::prometheus::client::MetricFamily>());
};

class RegistryTest : public Test {};

TEST_F(RegistryTest, collectsSingleMetricFamily) {
    auto registry = Registry{};
    auto counterFamily = registry.add_counter("test", "a test", {});
    counterFamily->add({{"name", "counter1"}});
    counterFamily->add({{"name", "counter2"}});
    auto collected = registry.collect();
    ASSERT_EQ(collected.size(), 1);
    EXPECT_EQ(collected[0].name(), "test");
    EXPECT_EQ(collected[0].help(), "a test");
    ASSERT_EQ(collected[0].metric_size(), 2);
    ASSERT_EQ(collected[0].metric(0).label_size(), 1);
    EXPECT_EQ(collected[0].metric(0).label(0).name(), "name");
    ASSERT_EQ(collected[0].metric(1).label_size(), 1);
    EXPECT_EQ(collected[0].metric(1).label(0).name(), "name");
}
