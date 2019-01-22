//
// Created by Gregor Jasny on 2019-01-22.
//

#include "metric_collector.h"

namespace prometheus {

    std::vector<prometheus::MetricFamily>
    CollectMetrics(const std::vector<std::weak_ptr<prometheus::Collectable>> &collectables) {
        auto collected_metrics = std::vector<prometheus::MetricFamily>{};

        for (auto &&wcollectable : collectables) {
            auto collectable = wcollectable.lock();
            if (!collectable) {
                continue;
            }

            auto &&metrics = collectable->Collect();
            collected_metrics.insert(collected_metrics.end(),
                                     std::make_move_iterator(metrics.begin()),
                                     std::make_move_iterator(metrics.end()));
        }

        return collected_metrics;
    }

}