// #include "Registries/ServerDetailsRegistry.hpp"
// #include "logging.hpp"
// #include "config.hpp"

// DEFINE_TYPE(BeatTogether::Registries, ServerDetailsRegistry);

// namespace BeatTogether::Registries {
    
//     void ServerDetailsRegistry::SetSelectedServer(BeatTogether::Models::ServerDetails* server) {
//         auto tmpServer = il2cpp_utils::try_cast<BeatTogether::Models::TemporaryServerDetails*>(server);

//         if (tmpServer.has_value())
//         {
//             TemporarySelectedServer = *tmpServer.value();
//         }
//         else
//         {
//             config.selectedServer = std::string(server->ServerName);
//             TemporarySelectedServer = nullptr;
//         }
//     }

//         // void ServerDetailsRegistry::ctor() {
//         //     // temporarySelectedServer = std::nullopt;
//         //     // servers = std::map<std::string, ServerDetails>();
//         // }
    
//         // ServerDetails ServerDetailsRegistry::get_SelectedServer() {
//         //     if (temporarySelectedServer.has_value())
//         //         return temporarySelectedServer.value();
//         //     else if (servers.find(config.selectedServer) != servers.end())
//         //         return servers[config.SelectedServer];
//         //     else
//         //         return servers[/*Config::BeatTogetherServerName*/"BeatTogether"];
//         // }

//         Models::ServerDetails* get_SelectedServer() {
//             if (TemporarySelectedServer)
//                 return TemporarySelectedServer;

//             auto it = std::find_if(Servers.begin(), Servers.end(), [this](auto x){
//                 return x && x->ServerName == config.selectedServer;
//             });
//             if (it != Servers.end())
//                 return *it;
//             else
//             auto it = std::find_if(Servers.begin(), Servers.end(), [this](auto x){
//                 return x && x->ServerName == StringW("BeatTogether");
//             });
//             if (it != Servers.end())
//                 return *it;
//             ERROR("Server not found: Could not find any server matches");
//             return nullptr;
//         }
// }