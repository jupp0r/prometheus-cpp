#include "prometheus/exposer.h"

#include <gtest/gtest.h>

#include "CivetServer.h"

namespace prometheus {
namespace {

using namespace testing;

TEST(ExposerTest, listenOnDistinctPorts) {
  Exposer firstExposer{"0.0.0.0:0"};
  auto firstExposerPorts = firstExposer.GetListeningPorts();
  ASSERT_EQ(1u, firstExposerPorts.size());
  EXPECT_NE(0, firstExposerPorts.front());

  Exposer secondExposer{"0.0.0.0:0"};
  auto secondExposerPorts = secondExposer.GetListeningPorts();
  ASSERT_EQ(1u, secondExposerPorts.size());
  EXPECT_NE(0, secondExposerPorts.front());

  EXPECT_NE(firstExposerPorts, secondExposerPorts);
}

TEST(ExposerTest, invalidExternalServer) {
  EXPECT_THROW(Exposer(std::shared_ptr<CivetServer>(nullptr)), std::invalid_argument);
}

}  // namespace
}  // namespace prometheus
