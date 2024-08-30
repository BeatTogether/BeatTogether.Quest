#include "hooking.hpp"
#include "logging.hpp"
#include "tasks.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "GlobalNamespace/NetworkUtility.hpp"
#include "GlobalNamespace/AuthenticationToken.hpp"
#include "GlobalNamespace/PlatformAuthenticationTokenProvider.hpp"

#include "GlobalNamespace/OculusPlatformUserModel.hpp"
#include "GlobalNamespace/UserInfo.hpp"

#include <sys/system_properties.h>

static ConstString dummy_auth("who_is_rem_?");

static std::string PicoBrand = "PICO";
static bool isPico = false;
static bool checkedForPico = false;

MAKE_AUTO_HOOK_ORIG_MATCH(PlatformAuthenticationTokenProvider_GetAuthenticationToken, &GlobalNamespace::PlatformAuthenticationTokenProvider::GetAuthenticationToken, 
System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken>*, GlobalNamespace::PlatformAuthenticationTokenProvider* self) {
    if (isPico) self->_platform = GlobalNamespace::AuthenticationToken::Platform(20); // Set platform to 20 for Pico

    auto t = PlatformAuthenticationTokenProvider_GetAuthenticationToken(self);
    return BeatTogether::StartTask<GlobalNamespace::AuthenticationToken>([=](){
        using namespace std::chrono_literals;
        while (!(t->IsCompleted || t->IsCanceled)) std::this_thread::sleep_for(50ms);
        GlobalNamespace::AuthenticationToken token = t->ResultOnSuccess;
        bool hasToken = !System::String::IsNullOrEmpty(token.sessionToken);
        if (token && hasToken) {
            INFO("Successfully got auth token, returning it!");
            if (token.platform == GlobalNamespace::AuthenticationToken::Platform::Oculus) 
                token.platform = GlobalNamespace::AuthenticationToken::Platform::OculusQuest; // Makes sure platform is set to OculusQuest
            return token;
        }
        else if (!hasToken && self->_platform.value__ != 20) ERROR("Session token is null or empty!!! Either they don't own the game or modded games no longer have access to tokens");
        WARNING("Failed to get auth token, returning custom authentication token!");
        return GlobalNamespace::AuthenticationToken(
            self->_platform == GlobalNamespace::AuthenticationToken::Platform::Oculus ? GlobalNamespace::AuthenticationToken::Platform::OculusQuest : self->_platform,
            self->_userId, // Important for server and client to keep track of players, should not be messed with if possible
            self->_userName,
            dummy_auth
        );
    });
}




MAKE_AUTO_HOOK_ORIG_MATCH(OculusPlatformUserModel_GetUserInfo, &GlobalNamespace::OculusPlatformUserModel::GetUserInfo, System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>*, GlobalNamespace::OculusPlatformUserModel* self, System::Threading::CancellationToken ctx) {;
    if (!checkedForPico) {
        auto prop = __system_property_find("ro.product.brand");
        if (prop)
            __system_property_read_callback(prop, [](void* cookie, const char* name, const char* value, uint32_t serial) {
                std::string brand(value);
                INFO("Device brand: {}", brand);
                isPico = std::search(brand.begin(), brand.end(), PicoBrand.begin(), PicoBrand.end(), [](char a, char b){
                    return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
                }) != brand.end();
                checkedForPico = true;
            }, nullptr);
    }

    auto t = OculusPlatformUserModel_GetUserInfo(self, ctx);
    return BeatTogether::StartTask<GlobalNamespace::UserInfo*>([=](){
        using namespace std::chrono_literals;
        while (!(t->IsCompleted || t->IsCanceled)) std::this_thread::sleep_for(50ms);
        GlobalNamespace::UserInfo* info = t->ResultOnSuccess;
        if (info) {
            INFO("Successfully got user info, returning it!");
            if (isPico)
            {
                INFO("User is on Pico, changing platform to 20");
                info->platform = GlobalNamespace::UserInfo::Platform(20);
            }
        }
        return info;
    });
}
