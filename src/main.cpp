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
#include "GlobalNamespace/DnsEndPoint.hpp"
#include "GlobalNamespace/MultiplayerModeSelectionViewController.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/MainSystemInit.hpp"
#include "GlobalNamespace/NetworkConfigSO.hpp"
#include "GlobalNamespace/ClientCertificateValidator.hpp"
#include "GlobalNamespace/QuickPlaySongPacksDropdown.hpp"
using namespace GlobalNamespace;

#include "System/Action_1.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Resources.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

#include "multiplayer-core/shared/ServerConfigManager.hpp"

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

Logger& getLogger();

static ModInfo modInfo;

class ModConfig {
    public:
        ModConfig() : hostname(HOST_NAME), port(PORT), statusUrl(STATUS_URL), button("Modded\nOnline") {}
        // Read MUST be called after load.
        void read(std::string_view filename) {
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
        inline std::string get_hostname() const {
            getLogger().info("Host name: %s", hostname.c_str());
            return hostname;
        }
        inline std::string get_button() const {
            return button;
        }
        inline std::string get_statusUrl() const {
            getLogger().info("Status URL: %s", statusUrl.c_str());
            return statusUrl;
        }
    private:
        int port;
        std::string hostname;
        std::string button;
        std::string statusUrl;
};

static ModConfig config;

Logger& getLogger()
{
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

//static auto customLevelPrefixLength = 13;
//
//// Helper method for concatenating two strings using the Concat(System.Object) method.
//Il2CppString* concatHelper(Il2CppString* src, Il2CppString* dst) {
//    static auto* concatMethod = il2cpp_utils::FindMethod(il2cpp_functions::defaults->string_class, "Concat", il2cpp_functions::defaults->string_class, il2cpp_functions::defaults->string_class);
//    return RET_DEFAULT_UNLESS(getLogger(), il2cpp_utils::RunMethod<Il2CppString*>((Il2CppObject*) nullptr, concatMethod, src, dst));
//}

MAKE_HOOK_MATCH(PlatformAuthenticationTokenProvider_GetAuthenticationToken, &PlatformAuthenticationTokenProvider::GetAuthenticationToken, System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken>*, PlatformAuthenticationTokenProvider* self)
{
    getLogger().debug("Returning custom authentication token!");
    return System::Threading::Tasks::Task_1<AuthenticationToken>::New_ctor(AuthenticationToken(
        AuthenticationToken::Platform::OculusQuest,
        self->userId, // Important for server and client to keep track of things, should not be modified
        self->userName,
        Array<uint8_t>::NewLength(0)
    ));
}

// Change the "Online" menu text to "Modded Online"
MAKE_HOOK_MATCH(MainMenuViewController_DidActivate, &MainMenuViewController::DidActivate, void, MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool systemScreenEnabling)
{  
    // Find the GameObject and get the component for the online button's text
    static ConstString searchPath = "MainContent/OnlineButton/Text";
    TMPro::TextMeshProUGUI* onlineButtonText = self->get_gameObject()->get_transform()->Find(searchPath)->get_gameObject()->GetComponent<TMPro::TextMeshProUGUI*>();

    // Set the "Modded Online" text every time so that it doesn't change back
    // If we fail to get any valid button text, crash verbosely.
    // TODO: This could be replaced with a non-intense crash, if we can ensure that DidActivate also works as intended.
    onlineButtonText->set_text(CRASH_UNLESS(StringW(config.get_button())));

    // MultiplayerCore::NetworkConfigHooks::UseMasterServer("master.beattogether.systems", config.port, "http://master.beattogether.systems/status");

    MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, systemScreenEnabling);
}

// // Disable QuickplaySongPacksOverrides if MQE is missing
// MAKE_HOOK_MATCH(QuickPlaySongPacksDropdown_LazyInit, &QuickPlaySongPacksDropdown::LazyInit, void, QuickPlaySongPacksDropdown* self) {
//     self->quickPlaySongPacksOverride = nullptr;
//     QuickPlaySongPacksDropdown_LazyInit(self);
// }


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

    il2cpp_functions::Init();

    MultiplayerCore::ServerConfigManager::UseMasterServer(config.get_hostname(), config.get_port(), config.get_statusUrl());

    INSTALL_HOOK(getLogger(), PlatformAuthenticationTokenProvider_GetAuthenticationToken);
    // INSTALL_HOOK(getLogger(), MainSystemInit_Init);
    // INSTALL_HOOK(getLogger(), ClientCertificateValidator_ValidateCertificateChainInternal);
    INSTALL_HOOK(getLogger(), MainMenuViewController_DidActivate);
    // Checks if MQE is installed
    const std::unordered_map<std::string, const Mod>& ModList = Modloader::getMods();
    if (ModList.find("MultiplayerCore") != ModList.end()) {
        getLogger().info("Hello MultiplayerCore!");
        getLogger().debug("MultiplayerCore detected!");
        if (ModList.find("MultiQuestensions") != ModList.end()) {
            getLogger().info("Hello MQE!");
            getLogger().debug("MQE detected!");
        }
    }
    // else {
    //     getLogger().warning("MultiplayerCore not found, CustomSongs will not work!");
    //     INSTALL_HOOK(getLogger(), QuickPlaySongPacksDropdown_LazyInit);
    // }
}
