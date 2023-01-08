#pragma once

#include "BeatTogether.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "HMUI/ViewController.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"

#include "Registries/ServerDetailsRegistry.hpp"

#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/JoiningLobbyViewController.hpp"

#include "bsml/shared/macros.hpp"
#include "bsml/shared/BSML/FloatingScreen/FloatingScreen.hpp"

static const std::vector<std::function<void(bool, bool, bool)>> DidActivate_AffinityHooks = {};
static const std::vector<std::function<void(bool, bool)>> DidDeactivate_AffinityHooks = {};
static const std::vector<std::function<bool(HMUI::ViewController*, HMUI::ViewController, HMUI::ViewController::AnimationType*)>> TopViewControllerWillChange_AffinityHooks = {};
static const std::vector<std::function<HMUI::ViewController::AnimationDirection*(HMUI::ViewController*, HMUI::ViewController*)>> DoPresentTransition_AffinityHooks = {};
static const std::vector<std::function<void(StringW&, StringW&)>> SetTitle_AffinityHooks = {};
static const std::vector<std::function<void(bool)>> SetGlobalUserInteraction_AffinityHooks = {};

___DECLARE_TYPE_WRAPPER_INHERITANCE(BeatTogether::UI, ServerSelectionController, Il2CppTypeEnum::IL2CPP_TYPE_CLASS, Il2CppObject, "BeatTogether::UI",
                                    { classof(::Zenject::IInitializable * ) }, 0, nullptr,
    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
    DECLARE_INJECT_METHOD(void, Inject, GlobalNamespace::MultiplayerModeSelectionFlowCoordinator* modeSelectionFlow, GlobalNamespace::JoiningLobbyViewController* joiningLobbyView, BeatTogether::Registries::ServerDetailsRegistry* serverRegistry);

    // Affinity "parity"
    DECLARE_INSTANCE_METHOD(void, Affinity_DidActivate, bool a, bool b, bool c);
    DECLARE_INSTANCE_METHOD(void, Affinity_DidDeactivate, bool a, bool b);
    DECLARE_INSTANCE_METHOD(bool, Affinity_TopViewControllerWillChange, HMUI::ViewController* oldViewController, HMUI::ViewController* newViewController,
            HMUI::ViewController::AnimationType* animationType);
    DECLARE_INSTANCE_METHOD(HMUI::ViewController::AnimationDirection*, Affinity_DoPresentTransition, HMUI::ViewController* toPresentViewController, HMUI::ViewController* toDismissViewController);
    DECLARE_INSTANCE_METHOD(void, Affinity_SetTitle, StringW value, StringW _title);
    DECLARE_INSTANCE_METHOD(void, Affinity_SetGlobalUserInteraction, bool _val);
public:
    void ApplySelectedServer(ServerDetails server);
    void SyncSelectedServer();
    void SyncTemporarySelectedServer();
)

static SafePtrUnity<BeatTogether::UI::ServerSelectionController> instance;