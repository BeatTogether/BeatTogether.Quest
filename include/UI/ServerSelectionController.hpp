#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "bsml/shared/BSML/Components/Settings/ListSetting.hpp"
#include "bsml/shared/BSML/FloatingScreen/FloatingScreen.hpp"

#include "multiplayer-core/shared/Models/MpStatusData.hpp"

#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/JoiningLobbyViewController.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(BeatTogether::UI, ServerSelectionController, System::Object, ::Zenject::IInitializable*, ::System::IDisposable*) {
    DECLARE_INSTANCE_FIELD(GlobalNamespace::MultiplayerModeSelectionFlowCoordinator*, modeSelectionFlow);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::JoiningLobbyViewController*, joiningLobbyView);

    DECLARE_INSTANCE_FIELD(bool, _interactable);
    DECLARE_INSTANCE_FIELD(bool, _globalInteraction);
    // DECLARE_INSTANCE_FIELD(uint8_t, _allowSelectionOnce);

    DECLARE_INSTANCE_FIELD(BSML::FloatingScreen*, _screen);
    DECLARE_INSTANCE_FIELD(BSML::ListSetting*, serverList);
    DECLARE_INSTANCE_FIELD(ListW<System::Object*>, serverOptions);


    DECLARE_INSTANCE_METHOD(System::Object*, get_server);
    DECLARE_INSTANCE_METHOD(void, set_server, System::Object* server);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &System::IDisposable::Dispose);

    DECLARE_CTOR(ctor, GlobalNamespace::MultiplayerModeSelectionFlowCoordinator* modeSelectionFlow, GlobalNamespace::JoiningLobbyViewController* joiningLobbyView);

    public:
        static ServerSelectionController* get_instance();

        void ApplySelectedServer(std::string server);
        // void ApplySelectedServer(ServerDetails server);
        // void SyncSelectedServer();
        // void ApplyNetworkConfig(ServerDetails server);
        // void SyncTemporarySelectedServer();
        // TODO: Requires implementation in MultiplayerCore.Quest
        // void HandleMpStatusUpdateForUrl(std::string url, MultiplayerCore::Models::MpStatusData* status);

        void SetInteraction(bool value);
        void SetGlobalUserInteraction(bool value);
        void SetTitle(StringW& value, StringW title);
        void Activate(bool firstActivation);
        void Deactivate();
        bool TopViewControllerWillChange(HMUI::ViewController* oldViewController, HMUI::ViewController* newViewController, HMUI::ViewController::AnimationType animationType);

    private:
        static ServerSelectionController* instance;
};
