#include "hooking.hpp"
#include "logging.hpp"
#include "tasks.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "GlobalNamespace/NetworkUtility.hpp"
#include "GlobalNamespace/AuthenticationToken.hpp"
#include "GlobalNamespace/PlatformAuthenticationTokenProvider.hpp"

static ConstString dummy_auth("who_is_rem_?");

MAKE_AUTO_HOOK_ORIG_MATCH(PlatformAuthenticationTokenProvider_GetAuthenticationToken, &GlobalNamespace::PlatformAuthenticationTokenProvider::GetAuthenticationToken, 
System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken>*, GlobalNamespace::PlatformAuthenticationTokenProvider* self) {
    if (self->_platform == GlobalNamespace::AuthenticationToken::Platform::PS4 || self->_platform == GlobalNamespace::AuthenticationToken::Platform::Test) {
        self->_platform = GlobalNamespace::AuthenticationToken::Platform(20); // If platform is set to PS4 or Test assume we run on Pico
        self->_hashedUserId = GlobalNamespace::NetworkUtility::GetHashedUserId(self->_userId, self->_platform);
    }

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
            // GlobalNamespace::AuthenticationToken::Platform::OculusQuest,
            self->_platform == GlobalNamespace::AuthenticationToken::Platform::Oculus ? GlobalNamespace::AuthenticationToken::Platform::OculusQuest : self->_platform,
            self->_userId, // Important for server and client to keep track of things, should not be modified
            self->_userName,
            dummy_auth
        );
    });
}
