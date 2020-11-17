#include "prometheus/metric_type.h"
#include "metrics_collector.h"
#define TESTING
#include "handler.h"
#undef TESTING

#include <gmock/gmock.h>

#ifdef MOCK_MG
/*
 * These symbols are hidden with cmake, but not bazel. Just dummy them up for the test.
 */
const char * mg_get_header(const struct mg_connection *conn, const char *name)
{
  (void) conn;
  (void) name;
  return nullptr;
}
int mg_printf(struct mg_connection *conn, const char *fmt, ...)
{
  (void) conn;
  (void) fmt;
  return 0;
}
int mg_write(struct mg_connection *conn, const void *buf, size_t len)
{
  (void) conn;
  (void) buf;
  (void) len;
  return 0;
}
#endif

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
