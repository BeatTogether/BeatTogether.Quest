#include "UI/ServerSelectionController.hpp"
#include "config.hpp"
#include "logging.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"

#include "GlobalNamespace/ConnectionErrorDialogViewController.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "HMUI/CurvedCanvasSettings.hpp"

#include "Polyglot/Localization.hpp"

#include "multiplayer-core/shared/MultiplayerCore.hpp"

DEFINE_TYPE(BeatTogether::UI, ServerSelectionController);

static inline UnityEngine::Vector3 operator*(UnityEngine::Vector3 vec, float val) {
    return {
        vec.x * val,
        vec.y * val,
        vec.z * val,
    };
}
namespace BeatTogether::UI {
    ServerSelectionController* ServerSelectionController::instance = nullptr;
    ServerSelectionController* ServerSelectionController::get_instance() {
        return instance;
    }

    void ServerSelectionController::ctor(GlobalNamespace::MultiplayerModeSelectionFlowCoordinator* modeSelectionFlow, GlobalNamespace::JoiningLobbyViewController* joiningLobbyView) {
        INVOKE_CTOR();

        this->modeSelectionFlow = modeSelectionFlow;
        this->joiningLobbyView = joiningLobbyView;
        serverOptions = ListW<System::Object*>::New();
        for (const auto& [serverName, server] : config.servers) serverOptions->Add(static_cast<System::Object*>(StringW(serverName).convert()));

        _interactable = true;
        _globalInteraction = true;

        instance = this;
    }

    void ServerSelectionController::Initialize() {
        _screen = BSML::FloatingScreen::CreateFloatingScreen({90, 90}, false, {0, 3, 4.35f}, {});
        BSML::parse_and_construct(Assets::ServerSelectionController_bsml, _screen->get_transform(), this);
        serverList->transform->GetChild(1).cast<UnityEngine::RectTransform>()->sizeDelta = {60, 0};
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

    void ServerSelectionController::Activate(bool firstActivation) {
        if (!firstActivation)
            _screen->get_gameObject()->SetActive(true);
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
        screenT->set_localScale(scale * extraYscale);

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

    System::Object* ServerSelectionController::get_server() {
        return static_cast<System::Object*>(StringW(config.selectedServer).convert());
    }

    void ServerSelectionController::set_server(System::Object* server) {
        ApplySelectedServer(StringW(server));
    }
}
