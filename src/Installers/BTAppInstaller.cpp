#include "Installers/BTAppInstaller.hpp"
#include "Registries/ServerDetailsRegistry.hpp"

#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

DEFINE_TYPE(BeatTogether::Installers, BTAppInstaller);

using namespace BeatTogether::Installers;

void BTAppInstaller::InstallBindings() {
    auto container = get_Container();

    container->BindInterfacesAndSelfTo<BeatTogether::Registries::ServerDetailsRegistry*>()->AsSingle();
}