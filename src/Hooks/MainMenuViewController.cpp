#include "hooking.hpp"
#include "logging.hpp"
#include "config.hpp"

#include "GlobalNamespace/MainMenuViewController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/Button.hpp"

// Change the "Online" menu text to "Modded Online"
MAKE_AUTO_HOOK_MATCH(MainMenuViewController_DidActivate, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool systemScreenEnabling) {
    auto onlineButtonText = self->_multiplayerButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
    if (onlineButtonText) onlineButtonText->set_text(config.button);
    else ERROR("Issue finding the online button text!");

    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, systemScreenEnabling);
}
