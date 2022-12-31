#pragma once

#include "BeatTogether.hpp"
#include "Models/ServerDetails.hpp"

#include <string>

static const std::string OfficialServerName = "Official Servers";
static const std::string BeatTogetherServerName = "BeatTogether";
static const std::string BeatTogetherHostName = "master.beattogether.systems";
static const std::string BeatTogetherStatusUri = "http://master.beattogether.systems/status";
static const int BeatTogetherMaxPartySize = 100;

DECLARE_CONFIG(BeatTogetherConfig,
               CONFIG_VALUE(servers, std::vector<ServerDetails>, "Servers", std::vector<ServerDetails>({
                   ServerDetails(BeatTogetherServerName, BeatTogetherHostName, BeatTogetherStatusUri, BeatTogetherMaxPartySize)
               }));
               CONFIG_VALUE(selectedServer, std::string, "SelectedServer", BeatTogetherServerName);
)

// Equiv: Config.cs -> public virtual void OnReload();
static void CheckConfig() {
    auto servers = getBeatTogetherConfig().servers.GetValue();

    bool foundYet = false;
    for (auto server : servers) {
        if(server.serverName == BeatTogetherServerName) {
            foundYet = true;
            break;
        }
    }

    if(!foundYet) {
        servers.emplace_back(BeatTogetherServerName, BeatTogetherHostName, BeatTogetherStatusUri, BeatTogetherMaxPartySize);
    }

    getBeatTogetherConfig().servers.SetValue(servers);
    getBeatTogetherConfig().Save();
}