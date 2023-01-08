#include "UI/ServerSelectionController.hpp"

DEFINE_TYPE(BeatTogether::UI, ServerSelectionController)

using namespace GlobalNamespace;
using namespace BeatTogether::Registries;
using namespace BSML;
using namespace HMUI;
using namespace BeatTogether;

#pragma region Affinity Impl Hooks

#include "HMUI/FlowCoordinator.hpp"

MAKE_HOOK_MATCH(AffinityPatch_FlowCoordinator_SetGlobalUserInteraction, &FlowCoordinator::SetGlobalUserInteraction, void, FlowCoordinator* self, bool val) {
    for (const auto &item: SetGlobalUserInteraction_AffinityHooks) {
        item(val);
    }

    AffinityPatch_FlowCoordinator_SetGlobalUserInteraction(self, val);
}

MAKE_HOOK_MATCH(AffinityPatch_FlowCoordinator_SetTitle, &FlowCoordinator::SetTitle, void, FlowCoordinator* self, StringW value, ViewController::AnimationType animType) {
    for (const auto &item: SetTitle_AffinityHooks) {
        item(value, self->title)
    }

    AffinityPatch_FlowCoordinator_SetTitle(self, value, animType);
}

#pragma endregion

#pragma region Class Impl

#pragma endregion