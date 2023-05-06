#pragma once

#include "multiplayer-core/shared/ServerConfig.hpp"

#ifndef HOST_NAME
#error "Define HOST_NAME!"
#endif

#ifndef PORT
#error "Define PORT!"
#endif

#ifndef STATUS_URL
#error "Define STATUS_URL!"
#endif

struct Config {
    MultiplayerCore::ServerConfig serverConfig = MultiplayerCore::ServerConfig(
        HOST_NAME, PORT, STATUS_URL
    );

    std::string button;
};

extern Config config;

bool LoadConfig();
void SaveConfig();
