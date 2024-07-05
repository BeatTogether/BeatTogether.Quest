#include "hooking.hpp"
#include "logging.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "GlobalNamespace/AuthenticationToken.hpp"
#include "GlobalNamespace/PlatformAuthenticationTokenProvider.hpp"

static ConstString dummy_auth("who_is_rem_?");

MAKE_AUTO_HOOK_ORIG_MATCH(PlatformAuthenticationTokenProvider_GetAuthenticationToken, &GlobalNamespace::PlatformAuthenticationTokenProvider::GetAuthenticationToken, System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken>*, GlobalNamespace::PlatformAuthenticationTokenProvider* self) {
    try {
        return PlatformAuthenticationTokenProvider_GetAuthenticationToken(self);
    } catch (const std::exception& e) {
        ERROR("Failed to get authentication token: {}", e.what());
        DEBUG("Returning custom authentication token!");
        return System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken>::New_ctor(GlobalNamespace::AuthenticationToken(
            GlobalNamespace::AuthenticationToken::Platform::OculusQuest,
            self->_userId, // Important for server and client to keep track of things, should not be modified
            self->_userName,
            dummy_auth
        ));
    }
}
