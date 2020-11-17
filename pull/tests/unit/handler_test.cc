#include "prometheus/metric_type.h"
#include "metrics_collector.h"
#define TESTING
#include "handler.h"
#undef TESTING

#include <gmock/gmock.h>

TEST(HandlerTest, removeCollectable) {

  std::shared_ptr<prometheus::Registry>  registry(std::make_shared<prometheus::Registry>());;
  prometheus::detail::MetricsHandler handler(*registry);

  std::shared_ptr<prometheus::Registry>  collectable(std::make_shared<prometheus::Registry>());

  auto& counter_family = prometheus::BuildCounter()
                             .Name("time_running_seconds_total")
                             .Help("How many seconds is this server running?")
                             .Labels({{"label", "value"}})
                             .Register(*collectable);
  (void)counter_family;

  handler.RegisterCollectable(collectable);

  std::vector<prometheus::MetricFamily> metrics;

  metrics = handler.getTestMetrics();

  ASSERT_EQ(1u, metrics.size());
  ASSERT_EQ("time_running_seconds_total", metrics[0].name);
  ASSERT_EQ("How many seconds is this server running?", metrics[0].help);
  ASSERT_EQ(prometheus::MetricType::Counter, metrics[0].type);

  handler.RemoveCollectable(collectable);

  metrics = handler.getTestMetrics();
  ASSERT_EQ(0u, metrics.size());
}
