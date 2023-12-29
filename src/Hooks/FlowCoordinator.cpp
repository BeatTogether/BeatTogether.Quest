#include "hooking.hpp"
#include "logging.hpp"

#include "UI/ServerSelectionController.hpp"

#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

MAKE_AUTO_HOOK_MATCH(FlowCoordinator_SetTitle, &::HMUI::FlowCoordinator::SetTitle, void, HMUI::FlowCoordinator* self, StringW value, HMUI::ViewController::AnimationType animationType) {
    auto controller = BeatTogether::UI::ServerSelectionController::get_instance();
    if (controller) {
        controller->SetTitle(value, self->title);
    }
    FlowCoordinator_SetTitle(self, value, animationType);
}

MAKE_AUTO_HOOK_MATCH(FlowCoordinator_SetGlobalUserInteraction, &::HMUI::FlowCoordinator::SetGlobalUserInteraction, void, HMUI::FlowCoordinator* self, bool value) {
    auto controller = BeatTogether::UI::ServerSelectionController::get_instance();
    if (controller) {
        controller->SetGlobalUserInteraction(value);
    }
    FlowCoordinator_SetGlobalUserInteraction(self, value);
}
