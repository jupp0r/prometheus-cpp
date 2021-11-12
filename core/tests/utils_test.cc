#include "prometheus/detail/utils.h"

#include <gtest/gtest.h>

#include <map>
#include <utility>

namespace prometheus {

namespace {

class UtilsTest : public testing::Test {
 public:
  detail::LabelHasher hasher;
};

TEST_F(UtilsTest, hash_labels_1) {
  std::map<std::string, std::string> labels;
  labels.insert(std::make_pair<std::string, std::string>("key1", "value1"));
  labels.insert(std::make_pair<std::string, std::string>("key2", "vaule2"));

  auto value1 = hasher(labels);
  auto value2 = hasher(labels);

  EXPECT_EQ(value1, value2);
}

TEST_F(UtilsTest, hash_labels_2) {
  std::map<std::string, std::string> labels1{{"aa", "bb"}};
  std::map<std::string, std::string> labels2{{"a", "abb"}};
  EXPECT_NE(hasher(labels1), hasher(labels2));
}

TEST_F(UtilsTest, hash_label_3) {
  std::map<std::string, std::string> labels1{{"a", "a"}};
  std::map<std::string, std::string> labels2{{"aa", ""}};
  EXPECT_NE(hasher(labels1), hasher(labels2));
}

}  // namespace

}  // namespace prometheus
