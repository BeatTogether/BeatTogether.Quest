#include "config.hpp"
#include "logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

extern ModInfo modInfo;
Config config;

Configuration& get_config() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

void SaveConfig() {
    // TODO: save config to configpath.cfg instead of configpath.json, like the mod used to do
    INFO("Saving config...");
    auto& cfg = get_config().config;
    cfg.RemoveAllMembers();
    cfg.SetObject();
    auto& allocator = cfg.GetAllocator();

    cfg.AddMember("button", config.button, allocator);
    cfg.AddMember("serverConfig", config.serverConfig.toJson(allocator), allocator);

    get_config().Write();
    INFO("Config saved!");
}

bool LoadConfig() {
    // TODO: load config from configpath.cfg instead of configpath.json, like the mod used to do
    INFO("Loading config...");
    bool foundEverything = true;

    auto& cfg = get_config().config;
    auto buttonItr = cfg.FindMember("button");
    if (buttonItr != cfg.MemberEnd() && buttonItr->value.IsString()) {
        config.button = {buttonItr->value.GetString(), buttonItr->value.GetStringLength()};
    } else {
        foundEverything = false;
    }

    auto serverConfigItr = cfg.FindMember("serverConfig");
    if (serverConfigItr != cfg.MemberEnd() && serverConfigItr->value.IsObject()) {
        if (!config.serverConfig.readJson(serverConfigItr->value)) foundEverything = false;
    } else {
        foundEverything = false;
    }
    if (foundEverything) INFO("Config loaded!");
    return foundEverything;
}
