#include "_config.h"
#include "config.hpp"
#include "hooking.hpp"
#include "scotland2/shared/loader.hpp"

#include "multiplayer-core/shared/MultiplayerCore.hpp"

#include "UI/ServerSelectionController.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"

modloader::ModInfo modInfo{MOD_ID, VERSION, VERSION_LONG};

BEATTOGETHER_EXPORT_FUNC void setup(CModInfo* info) {
    info->id = MOD_ID;
    info->version = VERSION;
    info->version_long = VERSION_LONG;
}

BEATTOGETHER_EXPORT_FUNC void late_load() {
    il2cpp_functions::Init();

    if (!LoadConfig())
        SaveConfig();
    custom_types::Register::AutoRegister();

    BeatTogether::Hooking::InstallHooks();

    MultiplayerCore::API::UseServer(config.GetSelectedConfig());

    auto zenjector = Lapiz::Zenject::Zenjector::Get();
    zenjector->Install(Lapiz::Zenject::Location::Menu, [](Zenject::DiContainer* container){
        container->BindInterfacesAndSelfTo<BeatTogether::UI::ServerSelectionController*>()->AsSingle();
    });
}
