#include "Registries/ServerDetailsRegistry.hpp"

DEFINE_TYPE(BeatTogether::Registries, ServerDetailsRegistry);

using namespace BeatTogether::Registries;
using namespace BeatTogether;

void ServerDetailsRegistry::ctor() {
    servers = getBeatTogetherConfig().servers.GetValue();
    servers.emplace_back(this->officialServer);

    auto iter = std::find_if(servers.begin(), servers.end(), [](const ServerDetails& details) {
        return details.serverName == getBeatTogetherConfig().selectedServer.GetValue();
    });

    if (iter == servers.end()) {
        iter = std::find_if(servers.begin(), servers.end(), [](const ServerDetails& details) {
            return details.serverName == BeatTogetherServerName;
        });
    }

    this->selectedServer = (this->_temporarySelectedServer.has_value() ? _temporarySelectedServer.value() : *iter);
}