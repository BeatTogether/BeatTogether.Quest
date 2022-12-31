#pragma once

#include "modloader/shared/modloader.hpp"

#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-type-check.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "config-utils/shared/config-utils.hpp"

#include "paper/shared/logger.hpp"

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "BTConfig.hpp"
#include "BTMacros.hpp"

static ModInfo modInfo;

#define INFO(...) Paper::Logger::fmtLog<Paper::LogLevel::INF>(__VA_ARGS__)
#define ERROR(...) Paper::Logger::fmtLog<Paper::LogLevel::ERR>(__VA_ARGS__)
#define CRITICAL(...) Paper::Logger::fmtLog<Paper::LogLevel::ERR>(__VA_ARGS__)
#define DEBUG(...) Paper::Logger::fmtLog<Paper::LogLevel::DBG>(__VA_ARGS__)

namespace BeatTogether {
    class Hooks {
    private:
        inline static std::vector<void (*)(Logger &logger)> installFuncs;

    public:
        static void AddInstallFunc(void (*installFunc)(Logger &logger)) {
            installFuncs.push_back(installFunc);
        }

        static void InstallHooks(Logger &logger) {
            for (auto installFunc : installFuncs) {
                installFunc(logger);
            }
        }
    };
}

#define BeatTogetherInstallHooks(func)                                             \
    struct __BeatTogetherRegister##func {                                          \
        __BeatTogetherRegister##func() { BeatTogether::Hooks::AddInstallFunc(func); }  \
    };                                                                         \
    static __BeatTogetherRegister##func __BeatTogetherRegisterInstance##func;