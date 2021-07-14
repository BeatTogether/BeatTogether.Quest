#include <iostream>
#include <fstream>
#include <string_view>

#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-type-check.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "System/Threading/Tasks/Task_1.hpp"

#include "GlobalNamespace/PlatformAuthenticationTokenProvider.hpp"
#include "GlobalNamespace/AuthenticationToken.hpp"
#include "GlobalNamespace/MasterServerEndPoint.hpp"
#include "GlobalNamespace/MenuRpcManager.hpp"
#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "GlobalNamespace/MultiplayerLevelLoader.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/MultiplayerModeSelectionViewController.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/MainSystemInit.hpp"
#include "GlobalNamespace/NetworkConfigSO.hpp"
#include "GlobalNamespace/HostLobbySetupViewController.hpp"
#include "GlobalNamespace/HostLobbySetupViewController_CannotStartGameReason.hpp"
#include "GlobalNamespace/AdditionalContentModel.hpp"
#include "GlobalNamespace/MultiplayerLevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/LevelSelectionFlowCoordinator_State.hpp"
#include "GlobalNamespace/SongPackMask.hpp"
#include "GlobalNamespace/BeatmapDifficultyMask.hpp"
#include "GlobalNamespace/UserCertificateValidator.hpp"
#include "GlobalNamespace/LevelSelectionNavigationController.hpp"
#include "GlobalNamespace/MultiplayerSessionManager.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
using namespace GlobalNamespace;

#include "System/Action_1.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Resources.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

//#include "Polyglot/LocalizedTextMeshProUGUI.hpp"
//#include "Polyglot/LanguageDirection.hpp"

//using namespace Polyglot;

#ifndef HOST_NAME
#error "Define HOST_NAME!"
#endif

#ifndef PORT
#error "Define PORT!"
#endif

#ifndef STATUS_URL
#error "Define STATUS_URL!"
#endif

// Needed for MPEX support
Il2CppString* getCustomSongsStateStr() {
    static auto* customSongStateStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("customsongs");
    return customSongStateStr;
}

Il2CppString* getFreeModStateStr() {
    static auto* freeModStateStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("freemod");
    return freeModStateStr;
}

Il2CppString* getHostPickStateStr() {
    static auto* hostPickStateStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("hostpick");
    return hostPickStateStr;
}

// std::string customSongsState = "customsongs";
// std::string freeModState = "freemod";
// std::string hostPickState = "hostpick";
bool customSongsEnabled = true;


Logger& getLogger();

static ModInfo modInfo;

class ModConfig {
    public:
        ModConfig() : hostname(HOST_NAME), port(PORT), statusUrl(STATUS_URL), button("Modded\nOnline") {}
        // Should be called after modification of the fields has already taken place.
        // Creates the C# strings for the configuration.
        void load() {
            createStrings();
        }
        // Read MUST be called after load.
        void read(std::string_view filename) {
            // Each time we read, we must start by cleaning up our old strings.
            // TODO: This may not be necessary if we only ever plan to load our configuration once.
            invalidateStrings();
            std::ifstream file(filename.data());
            if (!file) {
                getLogger().debug("No readable configuration at %s.", filename.data());
            } else {
                file >> hostname >> port >> statusUrl;
                button = hostname;
            }
            file.close();
        }
        inline int get_port() const {
            return port;
        }
        inline Il2CppString* get_hostname() const {
            getLogger().info("Host name: %s", to_utf8(csstrtostr(hostnameStr)).c_str());
            return valid ? hostnameStr : nullptr;
        }
        inline Il2CppString* get_button() const {
            return valid ? buttonStr : nullptr;
        }
        inline Il2CppString* get_statusUrl() const {
            getLogger().info("Status URL: %s", to_utf8(csstrtostr(statusUrlStr)).c_str());
            return valid ? statusUrlStr : nullptr;
        }
    private:
        // Invalidates all Il2CppString* pointers we have
        void invalidateStrings() {
            free(hostnameStr);
            free(buttonStr);
            free(statusUrlStr);
            valid = false;
        }
        // Creates all Il2CppString* pointers we need
        void createStrings() {
            hostnameStr = RET_V_UNLESS(getLogger(), il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>(hostname));
            buttonStr = RET_V_UNLESS(getLogger(), il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>(button));
            statusUrlStr = RET_V_UNLESS(getLogger(), il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>(statusUrl));
            // If we can make the strings okay, we are valid.
            valid = true;
        }
        bool valid;
        int port;
        std::string hostname;
        std::string button;
        std::string statusUrl;
        // C# strings of the configuration strings.
        // TODO: Consider replacing the C++ string fields entirely, as they serve no purpose outside of debugging.
        Il2CppString* hostnameStr = nullptr;
        Il2CppString* buttonStr = nullptr;
        Il2CppString* statusUrlStr = nullptr;
};

static ModConfig config;

Logger& getLogger()
{
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

static auto customLevelPrefixLength = 13;

// Helper method for concatenating two strings using the Concat(System.Object) method.
Il2CppString* concatHelper(Il2CppString* src, Il2CppString* dst) {
    static auto* concatMethod = il2cpp_utils::FindMethod(il2cpp_functions::defaults->string_class, "Concat", il2cpp_functions::defaults->string_class, il2cpp_functions::defaults->string_class);
    return RET_DEFAULT_UNLESS(getLogger(), il2cpp_utils::RunMethod<Il2CppString*>((Il2CppObject*) nullptr, concatMethod, src, dst));
}


Il2CppString* getCustomLevelStr() {
    static auto* customStr = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("custom_level_");
    return customStr;
}

MAKE_HOOK_MATCH(PlatformAuthenticationTokenProvider_GetAuthenticationToken, &PlatformAuthenticationTokenProvider::GetAuthenticationToken, System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken>*, PlatformAuthenticationTokenProvider* self)
{
    getLogger().debug("Returning custom authentication token!");
    return System::Threading::Tasks::Task_1<AuthenticationToken>::New_ctor(AuthenticationToken(
        AuthenticationToken::Platform::OculusQuest,
        self->userId,
        self->userName,
        Array<uint8_t>::NewLength(0)
    ));
}

MAKE_HOOK_MATCH(MainSystemInit_Init, &MainSystemInit::Init, void, MainSystemInit* self) {
    MainSystemInit_Init(self);
    auto* networkConfig = self->networkConfig;

    getLogger().info("Overriding master server end point . . .");
    getLogger().info("Original status URL: %s", to_utf8(csstrtostr(networkConfig->masterServerStatusUrl)).c_str());
    // If we fail to make the strings, we should fail silently
    // This could also be replaced with a CRASH_UNLESS call, if you want to fail verbosely.
    networkConfig->masterServerHostName = CRASH_UNLESS(/* getLogger(), */config.get_hostname());
    networkConfig->masterServerPort = CRASH_UNLESS(/* getLogger(), */config.get_port());
    networkConfig->masterServerStatusUrl = CRASH_UNLESS(/* getLogger(), */config.get_statusUrl());
}

MAKE_HOOK_MATCH(UserCertificateValidator_ValidateCertificateChainInternal, &UserCertificateValidator::ValidateCertificateChainInternal, void, UserCertificateValidator* self, GlobalNamespace::MasterServerEndPoint* endPoint, System::Security::Cryptography::X509Certificates::X509Certificate2* certificate, ::Array<::Array<uint8_t>*>* certificateChain)
{
    // TODO: Support disabling the mod if official multiplayer is ever fixed
    // It'd be best if we do certificate validation here...
    // but for now we'll just skip it.
}

// Disable the quick play button
MAKE_HOOK_MATCH(MultiplayerModeSelectionViewController_DidActivate, &MultiplayerModeSelectionViewController::DidActivate, void, MultiplayerModeSelectionViewController* self, bool firstActivation, bool addedToHierarchy, bool systemScreenEnabling)
{
    if (firstActivation)
    {
        static auto* searchPath = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("Buttons/QuickPlayButton");
        UnityEngine::Transform* transform = self->get_gameObject()->get_transform();
        UnityEngine::GameObject* quickPlayButton = transform->Find(searchPath)->get_gameObject();
        quickPlayButton->SetActive(false);
    }

    MultiplayerModeSelectionViewController_DidActivate(self, firstActivation, addedToHierarchy, systemScreenEnabling);
}

// Change the "Online" menu text to "Modded Online"
MAKE_HOOK_MATCH(MainMenuViewController_DidActivate, &MainMenuViewController::DidActivate, void, MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool systemScreenEnabling)
{   
    // Find the GameObject for the online button's text
    static auto* searchPath = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("MainContent/OnlineButton");
    static auto* textName = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("Text");
    UnityEngine::Transform* transform = self->get_gameObject()->get_transform();
    UnityEngine::GameObject* onlineButton = transform->Find(searchPath)->get_gameObject();
    UnityEngine::GameObject* onlineButtonTextObj = onlineButton->get_transform()->Find(textName)->get_gameObject();

    // Set the "Modded Online" text every time so that it doesn't change back
    TMPro::TextMeshProUGUI* onlineButtonText = onlineButtonTextObj->GetComponent<TMPro::TextMeshProUGUI*>();
    // If we fail to get any valid button text, crash verbosely.
    // TODO: This could be replaced with a non-intense crash, if we can ensure that DidActivate also works as intended.
    onlineButtonText->set_text(CRASH_UNLESS(config.get_button()));
    
    // Align the Text in the Center
    onlineButtonText->set_alignment(TMPro::TextAlignmentOptions::Center);

    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, systemScreenEnabling);
}

static bool isMissingLevel = false;

// This hook makes sure to grey-out the play button so that players can't start a level that someone doesn't have.
// This prevents crashes.
MAKE_HOOK_MATCH(HostLobbySetupViewController_SetPlayersMissingLevelText, &HostLobbySetupViewController::SetPlayersMissingLevelText, void, HostLobbySetupViewController* self, Il2CppString* playersMissingLevelText) {
    getLogger().info("HostLobbySetupViewController_SetPlayersMissingLevelText");
    if(playersMissingLevelText && !isMissingLevel) {
        getLogger().info("Disabling start game as missing level text exists . . .");
        isMissingLevel = true;
        self->SetStartGameEnabled(false, HostLobbySetupViewController::CannotStartGameReason::DoNotOwnSong);
    }   else if(!playersMissingLevelText && isMissingLevel)   {
        getLogger().info("Enabling start game as missing level text does not exist . . .");
        isMissingLevel = false;
        self->SetStartGameEnabled(true, HostLobbySetupViewController::CannotStartGameReason::None);
    }

    HostLobbySetupViewController_SetPlayersMissingLevelText(self, playersMissingLevelText);
}

// Prevent the button becoming shown when we're force disabling it, as pressing it would crash
MAKE_HOOK_MATCH(HostLobbySetupViewController_SetStartGameEnabled, &HostLobbySetupViewController::SetStartGameEnabled, void, HostLobbySetupViewController* self, bool startGameEnabled, HostLobbySetupViewController::CannotStartGameReason cannotStartGameReason) {
    getLogger().info("HostLobbySetupViewController_SetStartGameEnabled. Enabled: %d. Reason: %d", startGameEnabled, (int)cannotStartGameReason);
    if(isMissingLevel && cannotStartGameReason == HostLobbySetupViewController::CannotStartGameReason::None) {
        getLogger().info("Game attempted to enable the play button when the level was missing, stopping it!");
        startGameEnabled = false;
        cannotStartGameReason = HostLobbySetupViewController::CannotStartGameReason::DoNotOwnSong;
    }
    HostLobbySetupViewController_SetStartGameEnabled(self, startGameEnabled, cannotStartGameReason);
}

// Checks if a MPEX player has customsongs enabled or not
MAKE_HOOK_MATCH(MultiplayerSessionManager_HandlePlayerStateChanged, &MultiplayerSessionManager::HandlePlayerStateChanged, void, MultiplayerSessionManager* self, IConnectedPlayer* player) {
    MultiplayerSessionManager_HandlePlayerStateChanged(self, player);
    if (player->get_isConnectionOwner()) {
        customSongsEnabled = player->HasState(getCustomSongsStateStr());
    }
}

// Sends MPEX players flags about our setup
MAKE_HOOK_MATCH(MultiplayerSessionManager_Start, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {
    MultiplayerSessionManager_Start(self);

    // Added for compatability with MPEX
    self->SetLocalPlayerState(getCustomSongsStateStr(), customSongsEnabled);
    self->SetLocalPlayerState(getFreeModStateStr(), false);
    self->SetLocalPlayerState(getHostPickStateStr(), true);
}

 //Sets our bool for the custom levels tab in multiplayer if MQE isn't installed
MAKE_HOOK_MATCH(LevelSelectionNavigationController_Setup, &LevelSelectionNavigationController::Setup, void, LevelSelectionNavigationController* self,
    SongPackMask songPackMask, BeatmapDifficultyMask allowedBeatmapDifficultyMask, Array<GlobalNamespace::BeatmapCharacteristicSO*>* notAllowedCharacteristics, 
    bool hidePacksIfOneOrNone, bool hidePracticeButton, bool showPlayerStatsInDetailView, Il2CppString* actionButtonText, IBeatmapLevelPack* levelPackToBeSelectedAfterPresent, 
    SelectLevelCategoryViewController::LevelCategory startLevelCategory, IPreviewBeatmapLevel* beatmapLevelToBeSelectedAfterPresent, bool enableCustomLevels) {
    getLogger().info("LevelSelectionNavigationController_Setup enabling custom songs . . .");
    LevelSelectionNavigationController_Setup(self, songPackMask, allowedBeatmapDifficultyMask, notAllowedCharacteristics, hidePacksIfOneOrNone, hidePracticeButton, showPlayerStatsInDetailView,
                                             actionButtonText, levelPackToBeSelectedAfterPresent, startLevelCategory, beatmapLevelToBeSelectedAfterPresent, customSongsEnabled);
}

extern "C" void setup(ModInfo& info)
{
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
}


extern "C" void load()
{
    std::string path = Configuration::getConfigFilePath(modInfo);
    path.replace(path.length() - 4, 4, "cfg");

    getLogger().info("Config path: %s", path.c_str());
    config.read(path);
    // Load and create all C# strings after we attempt to read it.
    // If we failed to read it, we will have default values.
    // If we fail to create the strings, valid will be false.
    config.load();

    il2cpp_functions::Init();

    INSTALL_HOOK(getLogger(), PlatformAuthenticationTokenProvider_GetAuthenticationToken);
    INSTALL_HOOK(getLogger(), MainSystemInit_Init);
    INSTALL_HOOK(getLogger(), UserCertificateValidator_ValidateCertificateChainInternal);
    INSTALL_HOOK(getLogger(), MultiplayerModeSelectionViewController_DidActivate);
    INSTALL_HOOK(getLogger(), MainMenuViewController_DidActivate);
    // Checks if MQE is installed and if it is, skip installing our custom songs and our entitlement check hooks
    auto ModList = Modloader::getMods();
    if (ModList.find("multiquestensions") != ModList.end()) {
        getLogger().info("Hello MQE!");
        getLogger().debug("MQE detected, skipping some hooks!");
    }
    else {
        getLogger().info("MQE not found installing our safety hooks");
        INSTALL_HOOK(getLogger(), HostLobbySetupViewController_SetPlayersMissingLevelText);
        INSTALL_HOOK(getLogger(), HostLobbySetupViewController_SetStartGameEnabled);
        INSTALL_HOOK(getLogger(), LevelSelectionNavigationController_Setup);
        INSTALL_HOOK(getLogger(), MultiplayerSessionManager_Start);
        INSTALL_HOOK(getLogger(), MultiplayerSessionManager_HandlePlayerStateChanged);
    }
}
