#include <iostream>
#include <fstream>
#include <string_view>

#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
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
#include "GlobalNamespace/HostLobbySetupViewController.hpp"
#include "GlobalNamespace/HostLobbySetupViewController_CannotStartGameReason.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/MainSystemInit.hpp"
#include "GlobalNamespace/NetworkConfigSO.hpp"
#include "GlobalNamespace/AdditionalContentModel.hpp"

#include "UnityEngine/Resources.hpp"

using namespace GlobalNamespace;

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#ifndef HOST_NAME
#error "Define HOST_NAME!"
#endif

#ifndef PORT
#error "Define PORT!"
#endif

#ifndef STATUS_URL
#error "Define STATUS_URL!"
#endif

Logger& getLogger();

static ModInfo modInfo;

class ModConfig {
    public:
        ModConfig() : hostname(HOST_NAME), port(PORT), statusUrl(STATUS_URL), button("Modded Online") {}
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
        constexpr inline int get_port() const {
            return port;
        }
        constexpr inline Il2CppString* get_hostname() const {
            return valid ? hostnameStr : nullptr;
        }
        constexpr inline Il2CppString* get_button() const {
            return valid ? buttonStr : nullptr;
        }
        constexpr inline Il2CppString* get_statusUrl() const {
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
            hostnameStr = RET_V_UNLESS(getLogger(), il2cpp_utils::createcsstr(hostname, il2cpp_utils::StringType::Manual));
            buttonStr = RET_V_UNLESS(getLogger(), il2cpp_utils::createcsstr(button, il2cpp_utils::StringType::Manual));
            statusUrlStr = RET_V_UNLESS(getLogger(), il2cpp_utils::createcsstr(statusUrl, il2cpp_utils::StringType::Manual));
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
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

static auto customLevelPrefixLength = 13;

Il2CppString* getCustomLevelStr() {
    static auto* customStr = il2cpp_utils::createcsstr("custom_level_", il2cpp_utils::StringType::Manual);
    return customStr;
}

// Helper method for concatenating two strings using the Concat(System.Object) method.
Il2CppString* concatHelper(Il2CppString* src, Il2CppString* dst) {
    static auto* concatMethod = il2cpp_utils::FindMethod(il2cpp_functions::defaults->string_class, "Concat", std::vector<Il2CppClass*>{il2cpp_functions::defaults->object_class});
    return RET_DEFAULT_UNLESS(getLogger(), il2cpp_utils::RunMethod<Il2CppString*>(src, concatMethod, dst));
}

// Makes the Level ID stored in this identifer lower case if it is a custom level
void makeIdLowerCase(BeatmapIdentifierNetSerializable* identifier)
{
    // Check if it is a custom level
    if (identifier->levelID->StartsWith(getCustomLevelStr()))
        identifier->set_levelID(RET_V_UNLESS(getLogger(), concatHelper(getCustomLevelStr(), identifier->levelID->Substring(customLevelPrefixLength)->ToLower())));
}

// Makes the Level ID stored in this identifer upper case if it is a custom level
void makeIdUpperCase(BeatmapIdentifierNetSerializable* identifier)
{
    // Check if it is a custom level
    if (identifier->levelID->StartsWith(getCustomLevelStr()))
        identifier->set_levelID(RET_V_UNLESS(getLogger(), concatHelper(getCustomLevelStr(), identifier->levelID->Substring(customLevelPrefixLength)->ToUpper())));
}

MAKE_HOOK_OFFSETLESS(PlatformAuthenticationTokenProvider_GetAuthenticationToken, System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken>*, PlatformAuthenticationTokenProvider* self)
{
    getLogger().debug("Returning custom authentication token!");
    return System::Threading::Tasks::Task_1<AuthenticationToken>::New_ctor(AuthenticationToken(
        AuthenticationToken::Platform::OculusQuest,
        self->userId,
        self->userName,
        Array<uint8_t>::NewLength(0)
    ));
}

MAKE_HOOK_OFFSETLESS(MainSystemInit_Init, void, MainSystemInit* self) {
    MainSystemInit_Init(self);
    auto* networkConfig = self->networkConfig;

    getLogger().info("Overriding master server end point . . .");
    networkConfig->masterServerHostName = config.get_hostname();
    networkConfig->masterServerPort = config.get_port();
    networkConfig->masterServerStatusUrl = config.get_statusUrl();
}

MAKE_HOOK_OFFSETLESS(X509CertificateUtility_ValidateCertificateChainUnity, void, Il2CppObject* self, Il2CppObject* certificate, Il2CppObject* certificateChain)
{
    // TODO: Support disabling the mod if official multiplayer is ever fixed
    // It'd be best if we do certificate validation here...
    // but for now we'll just skip it.
}

MAKE_HOOK_OFFSETLESS(MenuRpcManager_SelectBeatmap, void, MenuRpcManager* self, BeatmapIdentifierNetSerializable* identifier)
{
    auto* levelID = identifier->get_levelID();
    makeIdUpperCase(identifier);
    MenuRpcManager_SelectBeatmap(self, identifier);
}

MAKE_HOOK_OFFSETLESS(MenuRpcManager_InvokeSelectedBeatmap, void, MenuRpcManager* self, Il2CppString* userId, BeatmapIdentifierNetSerializable* identifier)
{
    auto* levelID = identifier->get_levelID();
    makeIdLowerCase(identifier);
    MenuRpcManager_InvokeSelectedBeatmap(self, userId, identifier);
}

MAKE_HOOK_OFFSETLESS(MenuRpcManager_StartLevel, void, MenuRpcManager* self, BeatmapIdentifierNetSerializable* identifier, GameplayModifiers* gameplayModifiers, float startTime)
{
    auto* levelID = identifier->get_levelID();
    makeIdUpperCase(identifier);
    MenuRpcManager_StartLevel(self, identifier, gameplayModifiers, startTime);
}

MAKE_HOOK_OFFSETLESS(MenuRpcManager_InvokeStartLevel, void, MenuRpcManager* self, Il2CppString* userId, BeatmapIdentifierNetSerializable* identifier, GameplayModifiers* gameplayModifiers, float startTime)
{
    makeIdLowerCase(identifier);
    MenuRpcManager_InvokeStartLevel(self, userId, identifier, gameplayModifiers, startTime);
}

MAKE_HOOK_OFFSETLESS(MultiplayerLevelLoader_LoadLevel, void, MultiplayerLevelLoader* self, BeatmapIdentifierNetSerializable* beatmapId, GameplayModifiers* gameplayModifiers, float initialStartTime)
{
    // Change the ID to lower case temporarily so the level gets fetched correctly
    makeIdLowerCase(beatmapId);
    MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
    makeIdUpperCase(beatmapId);
}

// Disable the quick play button
MAKE_HOOK_OFFSETLESS(MultiplayerModeSelectionViewController_DidActivate, void, MultiplayerModeSelectionViewController* self, bool firstActivation, bool addedToHierarchy, bool systemScreenEnabling)
{
    if (firstActivation)
    {
        static auto* searchPath = il2cpp_utils::createcsstr("Buttons/QuickPlayButton", il2cpp_utils::StringType::Manual);
        UnityEngine::Transform* transform = self->get_gameObject()->get_transform();
        UnityEngine::GameObject* quickPlayButton = transform->Find(searchPath)->get_gameObject();
        quickPlayButton->SetActive(false);
    }

    MultiplayerModeSelectionViewController_DidActivate(self, firstActivation, addedToHierarchy, systemScreenEnabling);
}

// Change the "Online" menu text to "Modded Online"
MAKE_HOOK_OFFSETLESS(MainMenuViewController_DidActivate, void, MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool systemScreenEnabling)
{   
    // Find the GameObject for the online button's text
    static auto* searchPath = il2cpp_utils::createcsstr("MainButtons/OnlineButton", il2cpp_utils::StringType::Manual);
    static auto* textName = il2cpp_utils::createcsstr("Text", il2cpp_utils::StringType::Manual);
    UnityEngine::Transform* transform = self->get_gameObject()->get_transform();
    UnityEngine::GameObject* onlineButton = transform->Find(searchPath)->get_gameObject();
    UnityEngine::GameObject* onlineButtonTextObj = onlineButton->get_transform()->Find(textName)->get_gameObject();

    if (firstActivation)
    {
        // Move the text slightly to the right so it is centred
        UnityEngine::Vector3 currentTextPos = onlineButtonTextObj->get_transform()->get_position();
        currentTextPos.x += 0.025;
        onlineButtonTextObj->get_transform()->set_position(currentTextPos);
    }

    // Set the "Modded Online" text every time so that it doesn't change back
    TMPro::TextMeshProUGUI* onlineButtonText = onlineButtonTextObj->GetComponent<TMPro::TextMeshProUGUI*>();
    onlineButtonText->set_text(config.get_button());

    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, systemScreenEnabling);
}

// The base game's code always returns owned for a custom song, regardless of if we actually have it downloaded.
// We change this to only return owned if we actually have a level with that ID loaded.
// This fixes issues with people soft-locking if they don't have the song - players will be told if someone doesn't have it downloaded.
MAKE_HOOK_OFFSETLESS(AdditionalContentModel_GetLevelEntitlementStatusAsync, System::Threading::Tasks::Task_1<AdditionalContentModel::EntitlementStatus>*, AdditionalContentModel* self, Il2CppString* levelId, System::Threading::CancellationToken token) {
    static BeatmapLevelsModel* levelsModel = UnityEngine::Resources::FindObjectsOfTypeAll<BeatmapLevelsModel*>()->values[0];
    if(!levelsModel->IsBeatmapLevelLoaded(levelId)) {
        getLogger().info("custom song not owned!");
        return System::Threading::Tasks::Task_1<AdditionalContentModel::EntitlementStatus>::New_ctor(AdditionalContentModel::EntitlementStatus::NotOwned);
    }
    
    return AdditionalContentModel_GetLevelEntitlementStatusAsync(self, levelId, token);
}

// This hook makes sure to grey-out the play button so that players can't start a level that someone doesn't have.
// This prevents crashes.
MAKE_HOOK_OFFSETLESS(HostLobbySetupViewController_SetPlayersMissingLevelText, void, HostLobbySetupViewController* self, Il2CppString* playersMissingLevelText) {
    static bool wasPreviouslyHidden = false;

    if(playersMissingLevelText && !wasPreviouslyHidden) {
        self->SetStartGameEnabled(false, HostLobbySetupViewController::CannotStartGameReason::DoNotOwnSong);
        wasPreviouslyHidden = true;
    }   else if(!playersMissingLevelText && wasPreviouslyHidden)   {
        self->SetStartGameEnabled(true, HostLobbySetupViewController::CannotStartGameReason::None);
        wasPreviouslyHidden = false;
    }

    HostLobbySetupViewController_SetPlayersMissingLevelText(self, playersMissingLevelText);
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

    getLogger().info("Config path: " + path);
    config.read(path);
    // Load and create all C# strings after we attempt to read it.
    // If we failed to read it, we will have default values.
    // If we fail to create the strings, valid will be false.
    config.load();

    il2cpp_functions::Init();

    INSTALL_HOOK_OFFSETLESS(getLogger(), PlatformAuthenticationTokenProvider_GetAuthenticationToken,
        il2cpp_utils::FindMethod("", "PlatformAuthenticationTokenProvider", "GetAuthenticationToken"));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MainSystemInit_Init,
        il2cpp_utils::FindMethod("", "MainSystemInit", "Init"));
    INSTALL_HOOK_OFFSETLESS(getLogger(), X509CertificateUtility_ValidateCertificateChainUnity,
        il2cpp_utils::FindMethodUnsafe("", "X509CertificateUtility", "ValidateCertificateChainUnity", 2));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MenuRpcManager_SelectBeatmap,
        il2cpp_utils::FindMethodUnsafe("", "MenuRpcManager", "SelectBeatmap", 1));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MenuRpcManager_InvokeSelectedBeatmap,
        il2cpp_utils::FindMethodUnsafe("", "MenuRpcManager", "InvokeSelectedBeatmap", 2));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MenuRpcManager_StartLevel,
        il2cpp_utils::FindMethodUnsafe("", "MenuRpcManager", "StartLevel", 3));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MenuRpcManager_InvokeStartLevel,
        il2cpp_utils::FindMethodUnsafe("", "MenuRpcManager", "InvokeStartLevel", 4));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MultiplayerLevelLoader_LoadLevel,
        il2cpp_utils::FindMethodUnsafe("", "MultiplayerLevelLoader", "LoadLevel", 3));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MultiplayerModeSelectionViewController_DidActivate,
        il2cpp_utils::FindMethodUnsafe("", "MultiplayerModeSelectionViewController", "DidActivate", 3));
    INSTALL_HOOK_OFFSETLESS(getLogger(), MainMenuViewController_DidActivate, 
        il2cpp_utils::FindMethodUnsafe("", "MainMenuViewController", "DidActivate", 3));
    INSTALL_HOOK_OFFSETLESS(getLogger(), AdditionalContentModel_GetLevelEntitlementStatusAsync,
        il2cpp_utils::FindMethodUnsafe("", "AdditionalContentModel", "GetLevelEntitlementStatusAsync", 2));
    INSTALL_HOOK_OFFSETLESS(getLogger(), HostLobbySetupViewController_SetPlayersMissingLevelText,
        il2cpp_utils::FindMethodUnsafe("", "HostLobbySetupViewController", "SetPlayersMissingLevelText", 1));
}
