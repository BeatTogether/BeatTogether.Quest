#include "Installers/MenuInstaller.hpp"

#include "UI/ServerSelectionController.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

DEFINE_TYPE(BeatTogether::Installers, MenuInstaller);

namespace BeatTogether::Installers {
    void MenuInstaller::InstallBindings() {
        get_Container()->BindInterfacesAndSelfTo<UI::ServerSelectionController*>()->AsSingle();
    }
}
