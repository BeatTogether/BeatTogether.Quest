#include "config.hpp"
#include "logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

extern ModInfo modInfo;
Config config;

std::string get_configPath() {
    auto path = Configuration::getConfigFilePath(modInfo);
    return path.replace(path.length() - 4, 4, "cfg");
}

void SaveConfig() {
    INFO("Saving config...");

    rapidjson::Document cfg;

    cfg.SetObject();
    auto& allocator = cfg.GetAllocator();

    cfg.AddMember("button", config.button, allocator);
    cfg.AddMember("serverConfig", config.serverConfig.toJson(allocator), allocator);

    // doc to json string
    rapidjson::StringBuffer buff;
    rapidjson::Writer writer(buff);
    cfg.Accept(writer);

    writefile(get_configPath(), std::string_view(buff.GetString(), buff.GetSize()));

    INFO("Config saved!");
}

bool LoadConfig() {
    INFO("Loading config...");
    auto filePath = get_configPath();
    if (!fileexists(filePath)) {
        ERROR("Config file did not exist!");
        return false;
    }

    bool foundEverything = true;

    rapidjson::Document cfg;
    cfg.Parse(readfile(filePath));
    if (cfg.HasParseError() || !cfg.IsObject()) {
        ERROR("Failed to parse config as json document");
        return false;
    }

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

    config.serverConfig.maxPartySize = std::clamp(config.serverConfig.maxPartySize, 0, 25);

    if (foundEverything) INFO("Config loaded!");
    return foundEverything;
}
