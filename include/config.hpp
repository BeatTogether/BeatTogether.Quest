#pragma once

#include "multiplayer-core/shared/ServerConfig.hpp"

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
    MultiplayerCore::ServerConfig serverConfig = MultiplayerCore::ServerConfig(
        GRAPH_URL, STATUS_URL, MAX_PLAYER_COUNT
    );

    std::string button = "Modded\nOnline";
};

extern Config config;

bool LoadConfig();
void SaveConfig();
