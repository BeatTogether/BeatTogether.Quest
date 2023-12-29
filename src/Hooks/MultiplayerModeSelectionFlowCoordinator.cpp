#include "hooking.hpp"
#include "logging.hpp"

#include "UI/ServerSelectionController.hpp"

#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"

// may not run orig
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerModeSelectionFlowCoordinator_TopViewControllerWillChange, &::GlobalNamespace::MultiplayerModeSelectionFlowCoordinator::TopViewControllerWillChange, void, GlobalNamespace::MultiplayerModeSelectionFlowCoordinator* self, HMUI::ViewController* oldViewController, HMUI::ViewController* newViewController, HMUI::ViewController::AnimationType animationType) {
    auto controller = BeatTogether::UI::ServerSelectionController::get_instance();
    if (controller) {
        if (controller->TopViewControllerWillChange(oldViewController, newViewController, animationType))
            MultiplayerModeSelectionFlowCoordinator_TopViewControllerWillChange(self, oldViewController, newViewController, animationType);
    } else {
        MultiplayerModeSelectionFlowCoordinator_TopViewControllerWillChange(self, oldViewController, newViewController, animationType);
    }
}

MAKE_AUTO_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_DidActivate, &::GlobalNamespace::MultiplayerModeSelectionFlowCoordinator::DidActivate, void, GlobalNamespace::MultiplayerModeSelectionFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    auto controller = BeatTogether::UI::ServerSelectionController::get_instance();
    if (controller) {
        controller->Activate(firstActivation);
    }

    MultiplayerModeSelectionFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
}

MAKE_AUTO_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_DidDeactivate, &::GlobalNamespace::MultiplayerModeSelectionFlowCoordinator::DidDeactivate, void, GlobalNamespace::MultiplayerModeSelectionFlowCoordinator* self, bool removedFromHierarchy, bool screenSystemDisabling) {
    auto controller = BeatTogether::UI::ServerSelectionController::get_instance();
    if (controller) {
        controller->Deactivate();
    }

    MultiplayerModeSelectionFlowCoordinator_DidDeactivate(self, removedFromHierarchy, screenSystemDisabling);
}
