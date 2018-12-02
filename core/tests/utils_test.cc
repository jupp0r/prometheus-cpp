#include "prometheus/detail/utils.h"

#include <map>
#include <gmock/gmock.h>

namespace prometheus {

namespace {

void expect_not_equal(const std::map<std::string, std::string>& label1,
                      const std::map<std::string, std::string>& label2) {
  EXPECT_TRUE(utils::hash_labels(label1) != utils::hash_labels(label2));
}

TEST(UtilsTest, hash_labels_1) {
  std::map<std::string, std::string> labels;
  labels.insert(std::make_pair<std::string, std::string>("key1", "value1"));
  labels.insert(std::make_pair<std::string, std::string>("key2", "vaule2"));
  auto value1 = utils::hash_labels(labels);
  auto value2 = utils::hash_labels(labels);

  EXPECT_EQ(value1, value2);
}


TEST(UtilsTest, hash_labels_2) {
  std::map<std::string, std::string> labels1;
  labels1.insert(std::make_pair<std::string, std::string>("aa", "bb"));
  std::map<std::string, std::string> labels2;
  labels2.insert(std::make_pair<std::string, std::string>("a", "abb"));
  expect_not_equal(labels1, labels2);

  std::map<std::string, std::string> labels3;
  labels3.insert(std::make_pair<std::string, std::string>("a", "a"));
  std::map<std::string, std::string> labels4;
  labels4.insert(std::make_pair<std::string, std::string>("aa", ""));
  expect_not_equal(labels3, labels4);
}


}

}  //prometheus
