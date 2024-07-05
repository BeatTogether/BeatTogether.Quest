#pragma once

#include "multiplayer-core/shared/ServerConfig.hpp"
#include <string>
#include <map>

#ifndef GRAPH_URL
#error "Define GRAPH_URL!"
#endif

#ifndef STATUS_URL
#error "Define STATUS_URL!"
#endif

#ifndef MAX_PLAYER_COUNT
#define MAX_PLAYER_COUNT 10
#endif

struct Config {
    std::string button = "Modded\nOnline";

    std::string selectedServer = "BeatTogether";
    std::map<std::string, MultiplayerCore::ServerConfig> servers = std::map<std::string, MultiplayerCore::ServerConfig>({
        {"BeatTogether", {GRAPH_URL, STATUS_URL, MAX_PLAYER_COUNT, "", true}}
    });

    const MultiplayerCore::ServerConfig* GetSelectedConfig() const;
};

extern Config config;

bool LoadConfig();
void SaveConfig();
