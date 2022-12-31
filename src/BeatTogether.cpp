#include "BeatTogether.hpp"
#include "Installers/BTAppInstaller.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"

extern "C" void setup(ModInfo& info)
{
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
}

extern "C" void load()
{
    static auto *hookLogger = new Logger(modInfo);

    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();

    INFO("Loading Config...");
    getBeatTogetherConfig().Init(modInfo);
    CheckConfig();
    INFO("Loaded Config.");

    INFO("Preparing Zenject...");
    auto zenjector = Lapiz::Zenject::Zenjector::Get();

    zenjector->Install<BeatTogether::Installers::BTAppInstaller*>(Lapiz::Zenject::Location::App);
    INFO("Finished setting up installers.");

    INFO("Installing Hooks...");
    BeatTogether::Hooks::InstallHooks(*hookLogger);
    INFO("Finished installing hooks.")
}
