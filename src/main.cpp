#include "modloader/shared/modloader.hpp"
#include "config.hpp"
#include "hooking.hpp"

#include "multiplayer-core/shared/MultiplayerCore.hpp"

#include "UI/ServerSelectionController.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"

ModInfo modInfo{MOD_ID, VERSION};

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
    custom_types::Register::AutoRegister();

    auto& logger = getLogger();
    Hooks::InstallHooks(logger);

    MultiplayerCore::API::UseServer(config.GetSelectedConfig());

    // Checks if MQE is installed
    const std::unordered_map<std::string, const Mod>& ModList = Modloader::getMods();

    if (ModList.find("MultiQuestensions") != ModList.end()) {
        getLogger().info("Hello MQE!");
        getLogger().debug("MQE detected!");
    }

    auto zenjector = Lapiz::Zenject::Zenjector::Get();
    zenjector->Install(Lapiz::Zenject::Location::Menu, [](Zenject::DiContainer* container){
        container->BindInterfacesAndSelfTo<BeatTogether::UI::ServerSelectionController*>()->AsSingle();
    });
}
