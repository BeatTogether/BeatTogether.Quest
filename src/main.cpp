#include "modloader/shared/modloader.hpp"
#include "config.hpp"
#include "hooking.hpp"

#include "multiplayer-core/shared/MultiplayerCore.hpp"

static ModInfo modInfo{MOD_ID, VERSION};

Logger& getLogger() {
    static auto logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

extern "C" void setup(ModInfo& info) {
    info = modInfo;
}

extern "C" void load() {
    il2cpp_functions::Init();

    if (!LoadConfig())
        SaveConfig();

    auto& logger = getLogger();
    Hooks::InstallHooks(logger);

    MultiplayerCore::API::UseServer(&config.serverConfig);

    // Checks if MQE is installed
    const std::unordered_map<std::string, const Mod>& ModList = Modloader::getMods();

    if (ModList.find("MultiQuestensions") != ModList.end()) {
        getLogger().info("Hello MQE!");
        getLogger().debug("MQE detected!");
    }
}
