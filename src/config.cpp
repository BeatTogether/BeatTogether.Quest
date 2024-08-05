#include "config.hpp"
#include "logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

extern modloader::ModInfo modInfo;
Config config;

std::string get_configPath() {
    auto path = Configuration::getConfigFilePath(modInfo);
    return path.replace(path.length() - 4, 4, "json");
}

void SaveConfig() {
    INFO("Saving config...");

    rapidjson::Document cfg;

    cfg.SetObject();
    auto& allocator = cfg.GetAllocator();

    cfg.AddMember("SelectedServer", rapidjson::Value(config.selectedServer.c_str(), config.selectedServer.size(), allocator), allocator);

    rapidjson::Value serverArray;
    serverArray.SetArray();

    serverArray.Reserve(config.servers.size() - 1, allocator);
    for (const auto& [server, serverConfig] : config.servers) {
        if (server == config.officialServerName) continue; // Skip the official server when saving
        rapidjson::Value val;
        val.SetObject();

        val.AddMember("ServerName", server, allocator);
        val.AddMember("ApiUrl", serverConfig.graphUrl, allocator);
        val.AddMember("StatusUri", serverConfig.masterServerStatusUrl, allocator);
        val.AddMember("MaxPartySize", serverConfig.maxPartySize, allocator);
        val.AddMember("DisableSsl", serverConfig.disableSSL, allocator);

        serverArray.PushBack(val, allocator);
    }

    cfg.AddMember("Servers", serverArray, allocator);

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

    auto lastSelected = config.selectedServer;

    rapidjson::Document cfg;
    cfg.Parse(readfile(filePath));
    if (cfg.HasParseError() || !cfg.IsObject()) {
        ERROR("Failed to parse config as json document");
        return false;
    }

    auto selectedServerItr = cfg.FindMember("SelectedServer");
    if (selectedServerItr != cfg.MemberEnd() && selectedServerItr->value.IsString()) {
        config.selectedServer = {selectedServerItr->value.GetString(), selectedServerItr->value.GetStringLength()};
    } else {
        foundEverything = false;
    }

    auto serversItr = cfg.FindMember("Servers");
    if (serversItr != cfg.MemberEnd() && serversItr->value.IsArray() && !serversItr->value.GetArray().Empty()) {
        for (const auto& server : serversItr->value.GetArray()) {
            config.servers[server["ServerName"].GetString()] = {
                server["ApiUrl"].GetString(),
                server["StatusUri"].GetString(),
                server["MaxPartySize"].GetInt(),
                "",
                server.HasMember("DisableSsl") ? server["DisableSsl"].GetBool() : true
            };
        }
    } else {
        foundEverything = false;
    }

    // Check selected is in the list
    if (config.servers.find(config.selectedServer) == config.servers.end()) {
        WARNING("Selected server not found in config, using last selected {}", lastSelected);
        config.selectedServer = lastSelected;
        foundEverything = false;
    }

    if (foundEverything) INFO("Config loaded!");
    return foundEverything;
}

const MultiplayerCore::ServerConfig* Config::GetSelectedConfig() const {
    auto configItr = servers.find(selectedServer);
    if (configItr == servers.end()) return nullptr;
    return &configItr->second;
}