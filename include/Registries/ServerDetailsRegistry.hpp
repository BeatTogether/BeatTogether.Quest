#pragma once

#include "BeatTogether.hpp"
#include "Models/ServerDetails.hpp"

DECLARE_CLASS_CODEGEN(BeatTogether::Registries, ServerDetailsRegistry, Il2CppObject,
DECLARE_DEFAULT_CTOR();
public:
    ServerDetails selectedServer;
    std::vector<ServerDetails> servers;
private:
    std::vector<ServerDetails> _servers = {};
                      )