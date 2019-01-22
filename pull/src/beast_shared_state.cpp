#include "beast_shared_state.h"


BeastSharedState::BeastSharedState(std::string uri, std::vector<std::weak_ptr<prometheus::Collectable>>& collectables)
: uri_{uri}, collectables_{collectables}
{
}

