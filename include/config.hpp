#pragma once

#include "multiplayer-core/shared/ServerConfig.hpp"

#ifndef GRAPH_URL
#error "Define GRAPH_URL!"
#endif

#ifndef STATUS_URL
#error "Define STATUS_URL!"
#endif

struct Config {
    MultiplayerCore::ServerConfig serverConfig = MultiplayerCore::ServerConfig(
        GRAPH_URL, STATUS_URL
    );

    std::string button = "Modded\nOnline";
};

extern Config config;

bool LoadConfig();
void SaveConfig();
