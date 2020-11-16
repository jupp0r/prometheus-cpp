#include "prometheus/metric_type.h"
#include "metrics_collector.h"
#define TESTING
#include "handler.h"
#undef TESTING

#include <gmock/gmock.h>


/*
 * Dummies to resolve some callbacks that couldn't be mocked
 */
CIVETWEB_API const char *mg_get_header(const struct mg_connection *,
                                       const char *name) {return nullptr;}
CIVETWEB_API int mg_printf(struct mg_connection *,
                           PRINTF_FORMAT_STRING(const char *fmt),
                           ...) {return 0;}
CIVETWEB_API int mg_write(struct mg_connection *, const void *buf, size_t len) {return 0;}

/*
 * Mock the stuff not being used
 */
class MetricsHandlerTest : public prometheus::detail::MetricsHandler {
  public:
    MetricsHandlerTest(prometheus::Registry& registry) : prometheus::detail::MetricsHandler(registry) {}
    ~MetricsHandlerTest() {}

    MOCK_METHOD(bool, handleDelete, (CivetServer* server, struct mg_connection* conn), (override));
    MOCK_METHOD(bool, handleGet,    (CivetServer* server, struct mg_connection* conn), (override));
    MOCK_METHOD(bool, handleHead,   (CivetServer* server, struct mg_connection* conn), (override));
    MOCK_METHOD(bool, handleOptions,(CivetServer* server, struct mg_connection* conn), (override));
    MOCK_METHOD(bool, handlePatch,  (CivetServer* server, struct mg_connection* conn), (override));
    MOCK_METHOD(bool, handlePost,   (CivetServer* server, struct mg_connection* conn), (override));
    MOCK_METHOD(bool, handlePut,    (CivetServer* server, struct mg_connection* conn), (override));
};

TEST(HandlerTest, removeCollectable) {

  std::shared_ptr<prometheus::Registry>  registry(std::make_shared<prometheus::Registry>());;
  MetricsHandlerTest handler(*registry);

  std::shared_ptr<prometheus::Registry>  collectable(std::make_shared<prometheus::Registry>());

  auto& counter_family = prometheus::BuildCounter()
                             .Name("time_running_seconds_total")
                             .Help("How many seconds is this server running?")
                             .Labels({{"label", "value"}})
                             .Register(*collectable);
  handler.RegisterCollectable(collectable);

  std::vector<prometheus::MetricFamily> metrics;

  metrics = handler.getTestMetrics();

  ASSERT_EQ(1, metrics.size());
  ASSERT_EQ("time_running_seconds_total", metrics[0].name);
  ASSERT_EQ("How many seconds is this server running?", metrics[0].help);
  ASSERT_EQ(prometheus::MetricType::Counter, metrics[0].type);

  handler.RemoveCollectable(collectable);

  metrics = handler.getTestMetrics();
  ASSERT_EQ(0, metrics.size());
}
