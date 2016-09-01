#include "lib/exposer.h"

using namespace prometheus;

int main(int argc, char** argv) {
    auto server = Exposer{8080};
    server.run();
    return 0;
}
