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

void ServerDetailsRegistry::AddServer(ServerDetails server) {
    auto iter = std::find_if(servers.begin(), servers.end(), [&server](const ServerDetails& details) {
        return details.serverName == server.serverName;
    });

    if(iter != servers.end()) {
        throw std::invalid_argument(string_format("A server already exists with the name %s!", server.serverName.c_str()));
    } else {
        servers.emplace_back(server);
    }
}

void ServerDetailsRegistry::SetSelectedServer(const ServerDetails& server) {
    if(server.isTemporary) {
        this->_temporarySelectedServer = server;
    } else {
        getBeatTogetherConfig().selectedServer.SetValue(server.serverName);
        this->_temporarySelectedServer = std::nullopt;
    }
}