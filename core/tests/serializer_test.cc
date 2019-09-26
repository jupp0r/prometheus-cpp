#include "prometheus/counter.h"
#include "prometheus/family.h"
#include "prometheus/text_serializer.h"

#include <gmock/gmock.h>
#include <locale>
#include <sstream>

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

#ifndef _WIN32
TEST_F(SerializerTest, shouldSerializeLocaleIndependent) {
  // save and change locale
  const std::locale oldLocale = std::locale::classic();
  std::locale::global(std::locale("de_DE.UTF-8"));

  const auto serialized = textSerializer.Serialize(collected);
  EXPECT_THAT(serialized, testing::HasSubstr("1.0"));

  // restore locale
  std::locale::global(oldLocale);
}
#endif

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
