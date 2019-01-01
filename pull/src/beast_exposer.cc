#include "prometheus/beast_exposer.h"

#include <chrono>
#include <string>
#include <thread>

#include <boost/asio.hpp>

#include "beast_listener.h"

//#include "prometheus/client_metric.h"

//#include "CivetServer.h"
//#include "handler.h"

namespace prometheus {

    BeastExposer::BeastExposer(const std::string host, const std::uint16_t port, const std::string& uri, const std::size_t num_threads)
            : ioc_(new boost::asio::io_context(num_threads)),
              exposer_registry_(std::make_shared<Registry>()),
              //metrics_handler_(new detail::MetricsHandler{collectables_, *exposer_registry_}),
              uri_(uri) {
        RegisterCollectable(exposer_registry_);

        //
        auto& ioc = *ioc_;

        auto const address = boost::asio::ip::make_address(host);
        boost::asio::ip::tcp::endpoint endpoint{address, port};

        // Create and launch a listening port
        std::make_shared<listener>(
                ioc,
                endpoint,
                doc_root)->run();

        // Run the I/O service on the requested number of threads
        worker_.reserve(num_threads - 1);
        for(auto i = num_threads - 1; i > 0; --i) {
            worker_.emplace_back(
                    [&ioc]
                    {
                        ioc.run();
                    });
        }

        //server_->addHandler(uri, metrics_handler_.get());
    }

    BeastExposer::~BeastExposer() { //server_->removeHandler(uri_);
}

    void BeastExposer::RegisterCollectable(
            const std::weak_ptr<Collectable>& collectable) {
        collectables_.push_back(collectable);
    }
}  // namespace prometheus
