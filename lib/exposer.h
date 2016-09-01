#pragma once

#include <cstdint>

#include "CivetServer.h"

namespace prometheus {

class Exposer {
 public:
    Exposer(std::uint16_t port);
    void run();
  private:
    CivetServer server_;
};

}
