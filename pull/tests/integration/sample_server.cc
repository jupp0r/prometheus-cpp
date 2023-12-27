#include <array>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>

#include "prometheus/client_metric.h"
#include "prometheus/counter.h"
#include "prometheus/exposer.h"
#include "prometheus/family.h"
#include "prometheus/info.h"
#include "prometheus/registry.h"

#ifdef __linux__
#include <malloc.h>
#endif

int main() {
  using namespace prometheus;

  Exposer exposer{"127.0.0.1:8080"};

  // create a metrics registry
  // @note it's the users responsibility to keep the object alive
  auto registry = std::make_shared<Registry>();

  // add a new counter family to the registry (families combine values with the
  // same name, but distinct label dimensions)
  //
  // @note please follow the metric-naming best-practices:
  // https://prometheus.io/docs/practices/naming/
  auto& test_gauge = BuildGauge()
                         .Name("test_gauge")
                         .Help("Test gauge")
                         .Register(*registry);

  const char *labels[] = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten" };
  printf("Start adding data\n");
  clock_t start_time = clock();

  for(int i=0;i<200000;i++)
  {
    for(int j=0;j<10;j++)
    {
      std::string str = std::to_string(i);
      auto &tg = test_gauge.Add({{"label1",str}, {"label2",labels[j]}});
      tg.Set(i);
    }

  }

  printf("End adding data\n");
  clock_t end_time = clock();
  double duration = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  printf("duration: %f\n",duration);

  // ask the exposer to scrape the registry on incoming HTTP requests
  exposer.RegisterCollectable(registry);

  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(5));

#ifdef __linux__
    struct mallinfo2 mi;

    mi = mallinfo2();

    printf("Total non-mmapped bytes (arena):       %zu\n", mi.arena);
    printf("# of free chunks (ordblks):            %zu\n", mi.ordblks);
    printf("# of free fastbin blocks (smblks):     %zu\n", mi.smblks);
    printf("# of mapped regions (hblks):           %zu\n", mi.hblks);
    printf("Bytes in mapped regions (hblkhd):      %zu\n", mi.hblkhd);
    printf("Max. total allocated space (usmblks):  %zu\n", mi.usmblks);
    printf("Free bytes held in fastbins (fsmblks): %zu\n", mi.fsmblks);
    printf("Total allocated space (uordblks):      %zu\n", mi.uordblks);
    printf("Total free space (fordblks):           %zu\n", mi.fordblks);
    printf("Topmost releasable block (keepcost):   %zu\n", mi.keepcost);
#endif
  }
  return 0;
}
