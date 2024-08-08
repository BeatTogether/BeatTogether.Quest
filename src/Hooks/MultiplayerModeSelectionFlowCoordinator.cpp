#include "hooking.hpp"
#include "logging.hpp"

#include "UI/ServerSelectionController.hpp"

#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"

#include "System/Exception.hpp"

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

MAKE_AUTO_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_PresentMasterServerUnavailableErrorDialog, 
    &::GlobalNamespace::MultiplayerModeSelectionFlowCoordinator::PresentMasterServerUnavailableErrorDialog, 
    void, GlobalNamespace::MultiplayerModeSelectionFlowCoordinator* self, 
    ::GlobalNamespace::MultiplayerUnavailableReason reason, ::System::Exception* exception,
    ::System::Nullable_1<int64_t> maintenanceWindowEndTime,
    ::StringW remoteLocalizedMessage) 
{
    DEBUG("PresentMasterServerUnavailableErrorDialog hook called, with reason: {} and exception: {}", reason.value__, exception ? exception->Message : "nullptr");

    auto controller = BeatTogether::UI::ServerSelectionController::get_instance();
    if (controller) {
        controller->SetInteraction(true);
    }

    MultiplayerModeSelectionFlowCoordinator_PresentMasterServerUnavailableErrorDialog(self, reason, exception, maintenanceWindowEndTime, remoteLocalizedMessage);
}
