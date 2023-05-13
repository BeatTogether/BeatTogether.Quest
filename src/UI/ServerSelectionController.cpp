#include "UI/ServerSelectionController.hpp"
#include "config.hpp"
#include "logging.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"

#include "GlobalNamespace/ConnectionErrorDialogViewController.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"

#include "HMUI/ViewController_AnimationType.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"

#include "Polyglot/Localization.hpp"

#include "multiplayer-core/shared/MultiplayerCore.hpp"

DEFINE_TYPE(BeatTogether::UI, ServerSelectionController);

namespace BeatTogether::UI {
    ServerSelectionController* ServerSelectionController::instance = nullptr;
    ServerSelectionController* ServerSelectionController::get_instance() {
        return instance;
    }

    void ServerSelectionController::ctor(GlobalNamespace::MultiplayerModeSelectionFlowCoordinator* modeSelectionFlow, GlobalNamespace::JoiningLobbyViewController* joiningLobbyView) {
        INVOKE_CTOR();

        this->modeSelectionFlow = modeSelectionFlow;
        this->joiningLobbyView = joiningLobbyView;

        instance = this;
    }

    void ServerSelectionController::Initialize() {
        _screen = BSML::FloatingScreen::CreateFloatingScreen({90, 90}, false, {0, 3, 4.35f}, {});
        BSML::parse_and_construct(IncludedAssets::ServerSelectionController_bsml, _screen->get_transform(), this);
        reinterpret_cast<UnityEngine::RectTransform*>(serverList->get_transform()->GetChild(0))->set_sizeDelta({60, 0});
        _screen->GetComponent<HMUI::CurvedCanvasSettings*>()->SetRadius(140);
        _screen->get_gameObject()->SetActive(false);
    }

    void ServerSelectionController::Dispose() {
        if (instance == this) instance = nullptr;
    }

    void ServerSelectionController::ApplySelectedServer(std::string server) {
        DEBUG("Server changed to {}", server);

        auto oldServer = config.selectedServer;
        config.selectedServer = server;
        SaveConfig();
        auto cfg = config.GetSelectedConfig();
        if (!cfg) {
            ERROR("Can't set server {} as it did not exist in config", server);
            config.selectedServer = oldServer;
            SaveConfig();
            return;
        }

        MultiplayerCore::API::UseServer(cfg);

        SetInteraction(false);

        modeSelectionFlow->DidDeactivate(false, false);
        modeSelectionFlow->DidActivate(false, true, false);
        modeSelectionFlow->ReplaceTopViewController(joiningLobbyView, nullptr, HMUI::ViewController::AnimationType::None, HMUI::ViewController::AnimationDirection::Vertical);
    }

    void ServerSelectionController::Deactivate() {
        _screen->get_gameObject()->SetActive(false);
    }

    bool ServerSelectionController::TopViewControllerWillChange(HMUI::ViewController* oldViewController, HMUI::ViewController* newViewController, HMUI::ViewController::AnimationType animationType) {
        auto screenContainer = oldViewController ? oldViewController->get_transform()->get_parent()->get_parent() : newViewController->get_transform()->get_parent()->get_parent();
        auto screenSystem = screenContainer->get_parent();
        auto screenT = _screen->get_transform();
        auto scale = screenContainer->get_localScale();
        float extraYscale = screenSystem->get_localScale().y;
        scale.y *= extraYscale;
        screenT->set_localScale(scale);

        auto pos = screenContainer->get_position();
        pos.y += extraYscale * 1.15f;
        screenT->set_position(pos);
        _screen->get_gameObject()->SetActive(true);

        if (il2cpp_utils::try_cast<GlobalNamespace::MultiplayerModeSelectionViewController>(oldViewController).has_value())
            SetInteraction(false);
        if (il2cpp_utils::try_cast<GlobalNamespace::MultiplayerModeSelectionViewController>(newViewController).has_value() || il2cpp_utils::try_cast<GlobalNamespace::ConnectionErrorDialogViewController>(newViewController).has_value())
            SetInteraction(true);
        if (il2cpp_utils::try_cast<GlobalNamespace::JoiningLobbyViewController>(newViewController).has_value() && animationType == HMUI::ViewController::AnimationType::None)
            return false;
        return true;
    }

    void ServerSelectionController::SetTitle(StringW& value, StringW title) {
        if (Polyglot::Localization::Get("LABEL_CHECKING_SERVER_STATUS") == value) {
            value = Polyglot::Localization::Get("LABEL_MULTIPLAYER_MODE_SELECTION");
        }
        if (title == Polyglot::Localization::Get("LABEL_CHECKING_SERVER_STATUS") && value == "")
            SetInteraction(true);
    }

    void ServerSelectionController::SetInteraction(bool value) {
        _interactable = value;
        serverList->set_interactable(_interactable && _globalInteraction);
    }

    void ServerSelectionController::SetGlobalUserInteraction(bool value) {
        _globalInteraction = value;
        serverList->set_interactable(_interactable && _globalInteraction);
    }

    Il2CppObject* ServerSelectionController::get_server() {
        return StringW(config.selectedServer);
    }

    void ServerSelectionController::set_server(Il2CppObject* server) {
        ApplySelectedServer(StringW(server));
    }
}
