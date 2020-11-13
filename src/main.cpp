#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include "System/Threading/Tasks/Task_1.hpp"

#include "GlobalNamespace/PlatformAuthenticationTokenProvider.hpp"
#include "GlobalNamespace/AuthenticationToken.hpp"
#include "GlobalNamespace/MasterServerEndPoint.hpp"
#include "GlobalNamespace/MenuRpcManager.hpp"
#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "GlobalNamespace/MultiplayerLevelLoader.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"

using namespace GlobalNamespace;

static ModInfo modInfo;

const Logger& getLogger()
{
    static const Logger logger(modInfo);
    return logger;
}

extern "C" void setup(ModInfo& info)
{
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
}

// Makes the Level ID stored in this identifer lower case if it is a custom level
void makeIdLowerCase(BeatmapIdentifierNetSerializable* identifier) {
    // Check if it is a custom level
    if (identifier->levelID->StartsWith(il2cpp_utils::createcsstr("custom_level_")))
        identifier->set_levelID(il2cpp_utils::createcsstr("custom_level_" + to_utf8(csstrtostr(identifier->levelID->Substring(13)->ToLower()))));
}

// Makes the Level ID stored in this identifer upper case if it is a custom level
void makeIdUpperCase(BeatmapIdentifierNetSerializable* identifier) {
    // Check if it is a custom level
    if (identifier->levelID->StartsWith(il2cpp_utils::createcsstr("custom_level_")))
        identifier->set_levelID(il2cpp_utils::createcsstr("custom_level_" + to_utf8(csstrtostr(identifier->levelID->Substring(13)->ToUpper()))));
}

MAKE_HOOK_OFFSETLESS(PlatformAuthenticationTokenProvider_GetAuthenticationToken, System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken>*, PlatformAuthenticationTokenProvider* self)
{
    auto* sessionToken = Array<uint8_t>::NewLength(1);
    sessionToken->values[0] = 10;
    auto authenticationToken = AuthenticationToken(
        *reinterpret_cast<AuthenticationToken::Platform*>(self->platformUserModel),
        self->userId,
        self->userName,
        sessionToken
    );
    return System::Threading::Tasks::Task_1<AuthenticationToken>::New_ctor(authenticationToken);
}

MAKE_HOOK_OFFSETLESS(NetworkConfigSO_get_masterServerEndPoint, MasterServerEndPoint*, Il2CppObject* self)
{
    getLogger().debug("Patching master server end point (EndPoint='%s:%u').", HOST_NAME, PORT);
    static auto* hostName = il2cpp_utils::createcsstr(HOST_NAME, il2cpp_utils::StringType::Manual);
    return MasterServerEndPoint::New_ctor(hostName, PORT);
}

MAKE_HOOK_OFFSETLESS(X509CertificateUtility_ValidateCertificateChain, void, Il2CppObject* self, Il2CppObject* certificate, Il2CppObject* certificateChain)
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
    makeIdUpperCase(identifier);
    MenuRpcManager_InvokeSelectedBeatmap(self, userId, identifier);
}

MAKE_HOOK_OFFSETLESS(MenuRpcManager_StartLevel, void, MenuRpcManager* self, BeatmapIdentifierNetSerializable* identifier, GameplayModifiers* gameplayModifiers, float startTime)
{
    auto* levelID = identifier->get_levelID();
    if (levelID->StartsWith(il2cpp_utils::createcsstr("custom_level_")))
        identifier->set_levelID(il2cpp_utils::createcsstr("custom_level_" + to_utf8(csstrtostr(levelID->Substring(13)->ToUpper()))));
    MenuRpcManager_StartLevel(self, identifier, gameplayModifiers, startTime);
}

MAKE_HOOK_OFFSETLESS(MenuRpcManager_InvokeStartLevel, void, MenuRpcManager* self, Il2CppString* userId, BeatmapIdentifierNetSerializable* identifier, GameplayModifiers* gameplayModifiers, float startTime)
{
    auto* levelID = identifier->get_levelID();
    if (levelID->StartsWith(il2cpp_utils::createcsstr("custom_level_")))
        identifier->set_levelID(levelID->ToLower());
    MenuRpcManager_InvokeStartLevel(self, userId, identifier, gameplayModifiers, startTime);
}

MAKE_HOOK_OFFSETLESS(MultiplayerLevelLoader_LoadLevel, void, MultiplayerLevelLoader* self, BeatmapIdentifierNetSerializable* beatmapId, GameplayModifiers* gameplayModifiers, float initialStartTime) {
    // Change the ID to lower case temporarily so the level gets fetched correctly
    makeIdLowerCase(beatmapId);
    MultiplayerLevelLoader_LoadLevel(self, beatmapId, gameplayModifiers, initialStartTime);
    makeIdUpperCase(beatmapId);
}

extern "C" void load()
{
    il2cpp_functions::Init();

    INSTALL_HOOK_OFFSETLESS(PlatformAuthenticationTokenProvider_GetAuthenticationToken,
        il2cpp_utils::FindMethod("", "PlatformAuthenticationTokenProvider", "GetAuthenticationToken"));
    INSTALL_HOOK_OFFSETLESS(NetworkConfigSO_get_masterServerEndPoint,
        il2cpp_utils::FindMethod("", "NetworkConfigSO", "get_masterServerEndPoint"));
    INSTALL_HOOK_OFFSETLESS(X509CertificateUtility_ValidateCertificateChain,
        il2cpp_utils::FindMethodUnsafe("", "X509CertificateUtility", "ValidateCertificateChain", 2));
    INSTALL_HOOK_OFFSETLESS(MenuRpcManager_SelectBeatmap,
        il2cpp_utils::FindMethodUnsafe("", "MenuRpcManager", "SelectBeatmap", 1));
    INSTALL_HOOK_OFFSETLESS(MenuRpcManager_InvokeSelectedBeatmap,
        il2cpp_utils::FindMethodUnsafe("", "MenuRpcManager", "InvokeSelectedBeatmap", 2));
    INSTALL_HOOK_OFFSETLESS(MenuRpcManager_StartLevel,
        il2cpp_utils::FindMethodUnsafe("", "MenuRpcManager", "StartLevel", 3));
    INSTALL_HOOK_OFFSETLESS(MenuRpcManager_InvokeStartLevel,
        il2cpp_utils::FindMethodUnsafe("", "MenuRpcManager", "InvokeStartLevel", 4));
    INSTALL_HOOK_OFFSETLESS(MultiplayerLevelLoader_LoadLevel,
        il2cpp_utils::FindMethodUnsafe("", "MultiplayerLevelLoader", "LoadLevel", 3));
}
