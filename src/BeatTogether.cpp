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
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();

    INFO("Loading Config...");
    getBeatTogetherConfig().Init(modInfo);

    INFO("Loaded Config!");
    INFO("HOST_NAME: %s", getBeatTogetherConfig().hostName.GetValue());
    INFO("PORT: %i", getBeatTogetherConfig().port.GetValue());
    INFO("STATUS_URL: %s", getBeatTogetherConfig().status_url.GetValue());


    INFO("Preparing Zenject...");
    auto zenjector = Lapiz::Zenject::Zenjector::Get();

    zenjector->Install<BeatTogether::Installers::BTAppInstaller*>(Lapiz::Zenject::Location::App);
    INFO("Finished setting up installers.");
}
