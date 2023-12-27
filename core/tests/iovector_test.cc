#include "prometheus/iovector.h"

#include <gtest/gtest.h>

#include <memory>

namespace prometheus {
namespace {

class IOVectorTest : public testing::Test {
 public:
  IOVector iov;
};

TEST_F(IOVectorTest, emptyWhenNew) { EXPECT_TRUE(iov.empty()); }

TEST_F(IOVectorTest, emptyWhenVectorsEmpty) {
  iov.data.resize(3);
  EXPECT_TRUE(iov.empty());
}

TEST_F(IOVectorTest, notEmpty) {
  iov.data.push_back(IOVector::ByteVector{0xAB});
  EXPECT_FALSE(iov.empty());
}

TEST_F(IOVectorTest, sizeIsZeroForNew) { EXPECT_EQ(0L, iov.size()); }

TEST_F(IOVectorTest, sizeIsZeroWhenVectorsEmpty) {
  iov.data.resize(3);
  EXPECT_EQ(0L, iov.size());
}

TEST_F(IOVectorTest, addsChunkedData) {
  const auto chunkSize = 2U;
  iov.add("aa", chunkSize);
  iov.add("b", chunkSize);
  iov.add("bc", chunkSize);

  EXPECT_EQ(5U, iov.size());
  ASSERT_EQ(3U, iov.data.size());
  EXPECT_EQ(2U, iov.data.at(0).size());
  EXPECT_EQ(2U, iov.data.at(1).size());
  EXPECT_EQ(1U, iov.data.at(2).size());

  static constexpr std::size_t bufferSize = 6;
  char buffer[bufferSize + 1] = {};

  iov.copy(0L, buffer, bufferSize);
  EXPECT_STREQ("aabbc", buffer);
}

class IOVectorCopyTest : public IOVectorTest {
 public:
  void SetUp() override {
    iov.data.push_back(IOVector::ByteVector(1, 'a'));
    iov.data.push_back(IOVector::ByteVector(2, 'b'));
    iov.data.push_back(IOVector::ByteVector(3, 'c'));
  }

  static constexpr std::size_t bufferSize = 6;
  char buffer[bufferSize + 1] = {};
};

TEST_F(IOVectorCopyTest, countSize) { EXPECT_EQ(6L, iov.size()); }

TEST_F(IOVectorCopyTest, copiesData) {
  iov.copy(0L, buffer, bufferSize);
  EXPECT_STREQ("abbccc", buffer);
}

TEST_F(IOVectorCopyTest, copiesDataWithOffset) {
  iov.copy(2L, buffer, bufferSize);
  EXPECT_STREQ("bccc", buffer);
}

TEST_F(IOVectorCopyTest, copiesDataWithShortBuffer) {
  iov.copy(2L, buffer, 2);
  EXPECT_STREQ("bc", buffer);
}

TEST_F(IOVectorCopyTest, skipsEmptyChunks) {
  iov.data.at(1).clear();
  iov.copy(0L, buffer, bufferSize);
  EXPECT_STREQ("accc", buffer);
}

}  // namespace
}  // namespace prometheus
