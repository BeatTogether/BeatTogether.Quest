#pragma once

#include "BeatTogether.hpp"
#include "Models/ServerDetails.hpp"

extern const std::string OfficialServerName;

DECLARE_CLASS_CODEGEN(BeatTogether::Registries, ServerDetailsRegistry, Il2CppObject,
                    DECLARE_CTOR(ctor);
                    public:
                        ServerDetails selectedServer;
                        const ServerDetails officialServer = ServerDetails(OfficialServerName);
                        std::vector<ServerDetails> servers;
                        void AddServer(ServerDetails server);
                        void SetSelectedServer(ServerDetails server);
                    private:
                        std::vector<ServerDetails> _servers = {};
                        std::optional<ServerDetails> _temporarySelectedServer;
)