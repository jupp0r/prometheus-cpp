#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <boost/beast/core.hpp>

#include "prometheus/collectable.h"
#include "prometheus/registry.h"

class CivetServer;

namespace prometheus {

    namespace detail {
        class MetricsHandler;
    }  // namespace detail

    class BeastExposer {
    public:
        explicit BeastExposer(const std::string host, const std::uint16_t port,
                         const std::string& uri = std::string("/metrics"),
                         const std::size_t num_threads = 2);
        ~BeastExposer();
        void RegisterCollectable(const std::weak_ptr<Collectable>& collectable);

    private:
        std::vector<std::thread> worker_;
        std::unique_ptr<boost::asio::io_context> ioc_;
        std::vector<std::weak_ptr<Collectable>> collectables_;
        std::shared_ptr<Registry> exposer_registry_;
        //std::unique_ptr<detail::MetricsHandler> metrics_handler_;
        std::string uri_;
    };

}  // namespace prometheus
