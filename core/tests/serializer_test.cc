#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "prometheus/counter.h"
#include "prometheus/detail/future_std.h"
#include "prometheus/family.h"
#include "prometheus/metric_family.h"
#include "prometheus/text_serializer.h"

#if 0

namespace prometheus {
namespace {
class SerializerTest : public testing::Test {
 public:
  void SetUp() override {
    Family<Counter> family{"requests_total", "", {}};
    auto& counter = family.Add({});
    counter.Increment();

    collected = family.Collect();
  }

  std::vector<MetricFamily> collected;
  TextSerializer textSerializer;
};



TEST_F(SerializerTest, shouldRestoreStreamState) {
  std::ostringstream os;

  // save stream state
  auto saved_flags = os.flags();
  auto saved_precision = os.precision();
  auto saved_width = os.width();
  auto saved_fill = os.fill();
  auto saved_locale = os.getloc();

  // serialize
  textSerializer.Serialize(os, collected);

  // check for expected flags
  EXPECT_EQ(os.flags(), saved_flags);
  EXPECT_EQ(os.precision(), saved_precision);
  EXPECT_EQ(os.width(), saved_width);
  EXPECT_EQ(os.fill(), saved_fill);
  EXPECT_EQ(os.getloc(), saved_locale);
}

}  // namespace
}  // namespace prometheus
#endif