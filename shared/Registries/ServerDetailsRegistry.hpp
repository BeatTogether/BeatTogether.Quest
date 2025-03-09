// #include "Models/TemporaryServerDetails.hpp"
// #include "custom-types/shared/macros.hpp"
// #include "logging.hpp"
// #include "config.hpp"

// DECLARE_CLASS_CODEGEN(BeatTogether::Registries, ServerDetailsRegistry, Il2CppObject,
//     DECLARE_DEFAULT_CTOR();
//     // DECLARE_CTOR(_ctor);

//     DECLARE_INSTANCE_FIELD(BeatTogether::Models::TemporaryServerDetails*, TemporarySelectedServer);
//     DECLARE_INSTANCE_FIELD(ListW<BeatTogether::Models::ServerDetails*>, Servers);
//     DECLARE_INSTANCE_FIELD_DEFAULT(Models::ServerDetails*, OfficialServer, Models::ServerDetails::New_ctor());

//     DECLARE_INSTANCE_METHOD(void, SetSelectedServer, BeatTogether::Models::ServerDetails* server);
//     // private:
//         // std::optional<TemporaryServerDetails> temporarySelectedServer;
//         // std::map<std::string, ServerDetails> servers;
//     public:
//         Models::ServerDetails* get_SelectedServer();
//         __declspec(property(get = get_SelectedServer)) Models::ServerDetails SelectedServer;

// )