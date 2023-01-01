#pragma once

#include "BeatTogether.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "HMUI/ViewController.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"

#include "bsml/shared/macros.hpp"
#include "bsml/shared/BSML/FloatingScreen/FloatingScreen.hpp"

DECLARE_CLASS_CODEGEN(BeatTogether::UI, ServerSelectionController, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_METHOD(void, Awake);

    // Affinity "parity"
    DECLARE_INSTANCE_METHOD(void, Affinity_DidActivate);
    DECLARE_INSTANCE_METHOD(void, Affinity_DidDeactivate);
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