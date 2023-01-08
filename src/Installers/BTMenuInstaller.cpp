#include "Installers/BTMenuInstaller.hpp"
#include "UI/ServerSelectionController.hpp"

#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

DEFINE_TYPE(BeatTogether::Installers, BTMenuInstaller);

using namespace BeatTogether::Installers;

void BTMenuInstaller::InstallBindings() {
    auto container = get_Container();

    container->BindInterfacesAndSelfTo<BeatTogether::UI::ServerSelectionController*>()->AsSingle();
}