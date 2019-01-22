#pragma once

#include <memory>
#include <string>
#include <vector>

#include "prometheus/collectable.h"

class BeastSharedState
{
public:
    BeastSharedState(std::string uri, std::vector<std::weak_ptr<prometheus::Collectable>>& collectables);

    const std::string& get_uri() const { return uri_; }
    const std::vector<std::weak_ptr<prometheus::Collectable>>& get_collectables() const { return collectables_; }

private:
    const std::string uri_;
    const std::vector<std::weak_ptr<prometheus::Collectable>>& collectables_;
};
