#include "hooking.hpp"
#include "logging.hpp"
#include "tasks.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "GlobalNamespace/AuthenticationToken.hpp"
#include "GlobalNamespace/PlatformAuthenticationTokenProvider.hpp"

static ConstString dummy_auth("who_is_rem_?");

MAKE_AUTO_HOOK_ORIG_MATCH(PlatformAuthenticationTokenProvider_GetAuthenticationToken, &GlobalNamespace::PlatformAuthenticationTokenProvider::GetAuthenticationToken, System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken>*, GlobalNamespace::PlatformAuthenticationTokenProvider* self) {
    auto t = PlatformAuthenticationTokenProvider_GetAuthenticationToken(self);
    return BeatTogether::StartTask<GlobalNamespace::AuthenticationToken>([=](){
        using namespace std::chrono_literals;
        while (!(t->IsCompleted || t->IsCanceled)) std::this_thread::sleep_for(50ms);
        GlobalNamespace::AuthenticationToken token = t->ResultOnSuccess;
        if (token) {
            INFO("Successfully got auth token, returning it!");
            token.platform = GlobalNamespace::AuthenticationToken::Platform::OculusQuest; // Makes sure platform is set to OculusQuest
            return token;
        }
        ERROR("Failed to get auth token, returning custom authentication token!");
        return GlobalNamespace::AuthenticationToken(
            GlobalNamespace::AuthenticationToken::Platform::OculusQuest,
            self->_userId, // Important for server and client to keep track of things, should not be modified
            self->_userName,
            dummy_auth
        );
    });
}
