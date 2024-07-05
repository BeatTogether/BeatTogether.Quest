#pragma once

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include <vector>

namespace BeatTogether {
    class Hooking {
    private:
        inline static std::vector<void (*)()> installFuncs;

    public:
        static void AddInstallFunc(void (*installFunc)()) {
            installFuncs.push_back(installFunc);
        }

        static inline void InstallHooks() {
            for (auto& func : installFuncs) func();
        }
    };

    template<auto mPtr>
    concept has_metadata = requires() {
        { ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::methodInfo() } -> std::same_as<MethodInfo const*>;
    };

    template<auto mPtr>
    requires(has_metadata<mPtr>)
    using Metadata = ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>;

    /// @brief checks whether the function is match hookable, which requires the function to be at least 5 (5 * 4 = 20 bytes) instructions and not have an address of 0 (abstract/virtual funcs)
    template<auto mPtr>
    concept match_hookable = has_metadata<mPtr> && Metadata<mPtr>::size >= (0x5 * sizeof(int32_t)) && Metadata<mPtr>::addrs != 0x0;
}

#define AUTO_INSTALL_PATCH(name_) \
    struct Auto_Patch_##name_ {                                                 \
        Auto_Patch_##name_() {                                                  \
            ::BeatTogether::Hooking::AddInstallFunc(Patch_##name_);          \
        }                                                                       \
    };                                                                          \
    static Auto_Patch_##name_ Auto_Patch_Instance_##name_

#define HOOK_AUTO_INSTALL_ORIG(name_)                                                                       \
    struct Auto_Hook_##name_ {                                                                              \
        static void Auto_Hook_##name_##_Install() {                                                         \
            static constexpr auto logger = Paper::ConstLoggerContext(MOD_ID "_Install_" #name_);            \
            ::Hooking::InstallOrigHook<Hook_##name_>(logger);                                               \
        }                                                                                                   \
        Auto_Hook_##name_() { ::BeatTogether::Hooking::AddInstallFunc(Auto_Hook_##name_##_Install); }    \
    };                                                                                                      \
    static Auto_Hook_##name_ Auto_Hook_Instance_##name_

#define HOOK_AUTO_INSTALL(name_)                                                                            \
    struct Auto_Hook_##name_ {                                                                              \
        static void Auto_Hook_##name_##_Install() {                                                         \
            static constexpr auto logger = Paper::ConstLoggerContext(MOD_ID "_Install_" #name_);            \
            ::Hooking::InstallHook<Hook_##name_>(logger);                                                   \
        }                                                                                                   \
        Auto_Hook_##name_() { ::BeatTogether::Hooking::AddInstallFunc(Auto_Hook_##name_##_Install); }    \
    };                                                                                                      \
    static Auto_Hook_##name_ Auto_Hook_Instance_##name_

#define MAKE_AUTO_HOOK_MATCH(name_, mPtr, retval, ...)                                                                                              \
    struct Hook_##name_                                                                                                                             \
    {                                                                                                                                               \
        using funcType = retval (*)(__VA_ARGS__);                                                                                                   \
        static_assert(BeatTogether::match_hookable<mPtr>);                                                                                       \
        static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
        constexpr static const char* name() { return #name_; }                                                                                      \
        static const MethodInfo* getInfo() { return ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::methodInfo(); }                        \
        static funcType* trampoline() { return &name_; }                                                                                            \
        static inline retval (*name_)(__VA_ARGS__) = nullptr;                                                                                       \
        static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; }                                           \
        static retval hook_##name_(__VA_ARGS__);                                                                                                    \
    };                                                                                                                                              \
    HOOK_AUTO_INSTALL(name_);                                                                                                                       \
    retval Hook_##name_::hook_##name_(__VA_ARGS__)

#define MAKE_AUTO_HOOK_FIND_VERBOSE(name_, infoGet, retval, ...)                                            \
    struct Hook_##name_ {                                                                                   \
        constexpr static const char* name() { return #name_; }                                              \
        static const MethodInfo* getInfo() { return infoGet; }                                              \
        using funcType = retval (*)(__VA_ARGS__);                                                           \
        static funcType* trampoline() { return &name_; }                                                    \
        static inline retval (*name_)(__VA_ARGS__) = nullptr;                                               \
        static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; }   \
        static retval hook_##name_(__VA_ARGS__);                                                            \
    };                                                                                                      \
    HOOK_AUTO_INSTALL_ORIG(name_);                                                                          \
    retval Hook_##name_::hook_##name_(__VA_ARGS__)

#define MAKE_AUTO_HOOK_ORIG_MATCH(name_, mPtr, retval, ...)                                                                                         \
    struct Hook_##name_                                                                                                                             \
    {                                                                                                                                               \
        using funcType = retval (*)(__VA_ARGS__);                                                                                                   \
        static_assert(BeatTogether::match_hookable<mPtr>);                                                                                       \
        static_assert(std::is_same_v<funcType, ::Hooking::InternalMethodCheck<decltype(mPtr)>::funcType>, "Hook method signature does not match!"); \
        constexpr static const char* name() { return #name_; }                                                                                      \
        static const MethodInfo* getInfo() { return ::il2cpp_utils::il2cpp_type_check::MetadataGetter<mPtr>::methodInfo(); }                        \
        static funcType* trampoline() { return &name_; }                                                                                            \
        static inline retval (*name_)(__VA_ARGS__) = nullptr;                                                                                       \
        static funcType hook() { return &::Hooking::HookCatchWrapper<&hook_##name_, funcType>::wrapper; }                                           \
        static retval hook_##name_(__VA_ARGS__);                                                                                                    \
    };                                                                                                                                              \
    HOOK_AUTO_INSTALL_ORIG(name_);                                                                                                                  \
    retval Hook_##name_::hook_##name_(__VA_ARGS__)
