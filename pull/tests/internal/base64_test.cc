#include "detail/base64.h"

#include <gtest/gtest.h>

#include <string>

namespace prometheus {
namespace {

struct TestVector {
  const std::string decoded;
  const std::string encoded;
};

const TestVector testVector[] = {
    {"", ""},
    {"f", "Zg=="},
    {"fo", "Zm8="},
    {"foo", "Zm9v"},
    {"foob", "Zm9vYg=="},
    {"fooba", "Zm9vYmE="},
    {"foobar", "Zm9vYmFy"},
};

using namespace testing;

TEST(Base64Test, decodeTest) {
  for (const auto& test_case : testVector) {
    std::string decoded = detail::base64_decode(test_case.encoded);
    EXPECT_EQ(test_case.decoded, decoded);
  }
}

TEST(Base64Test, rejectInvalidSymbols) {
  EXPECT_ANY_THROW(detail::base64_decode("...."));
}

TEST(Base64Test, rejectInvalidInputSize) {
  EXPECT_ANY_THROW(detail::base64_decode("ABC"));
}

TEST(Base64Test, rejectInvalidPadding) {
  EXPECT_ANY_THROW(detail::base64_decode("A==="));
}

}  // namespace
}  // namespace prometheus
