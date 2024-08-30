#pragma once

#include "multiplayer-core/shared/MultiplayerCore.hpp"
#include <string>
#include <map>

#ifndef MAX_PLAYER_COUNT
#define MAX_PLAYER_COUNT 10
#endif

struct Config {
#if defined(SERVER_NAME)
    std::string selectedServer = SERVER_NAME;
#else
    std::string selectedServer = "BeatTogether";
#endif
    std::string officialServerName = "Official Servers";
    std::map<std::string, MultiplayerCore::ServerConfig> servers = std::map<std::string, MultiplayerCore::ServerConfig>({
        {"BeatTogether", {"http://master.beattogether.systems:8989", "http://master.beattogether.systems/status", MAX_PLAYER_COUNT, "", true}},
        #if defined(GRAPH_URL) && defined(STATUS_URL) && defined(SERVER_NAME)
        {SERVER_NAME, {GRAPH_URL, STATUS_URL, MAX_PLAYER_COUNT, "", true}},
        #endif
        {officialServerName, MultiplayerCore::API::GetOfficialServer() ? *MultiplayerCore::API::GetOfficialServer() : MultiplayerCore::ServerConfig()}
    });

    const MultiplayerCore::ServerConfig* GetSelectedConfig() const;
};

extern Config config;

bool LoadConfig();
void SaveConfig();
