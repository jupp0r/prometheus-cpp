#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "lib/exposer.h"
#include "lib/registry.h"

int main(int argc, char** argv) {
  using namespace prometheus;

  auto exposer = Exposer{8080};
  auto registry = std::make_shared<Registry>(std::map<std::string, std::string>{{"component", "main"}});
  auto counterFamily = registry->add_counter(
      "time_running_seconds", "How many seconds is this server running?", {});
  auto secondCounter = counterFamily->add({});
  exposer.registerCollectable(registry);
  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    secondCounter->inc();
  }
  return 0;
}
