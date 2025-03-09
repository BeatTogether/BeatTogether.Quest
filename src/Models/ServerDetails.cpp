#include "Models/ServerDetails.hpp"
#include "System/Uri.hpp"
#include "config.hpp"

// DEFINE_TYPE(BeatTogether::Models, ServerDetails);

namespace BeatTogether::Models {
    ServerDetails::ServerDetails(
        std::string serverName,
        std::string hostName,
        std::string apiUrl,
        std::string statusUrl,
        int maxPartySize,
        bool disableSSL
    ) {
        ServerName = serverName;
        HostName = hostName;
        graphUrl = apiUrl;
        StatusUrl = statusUrl;
        MaxPartySize = maxPartySize;
        DisableSSL = disableSSL;
    }

    std::string ServerDetails::get_ApiUrl() {
        return this->graphUrl;
    }

    void ServerDetails::set_ApiUrl(std::string value) {
        this->graphUrl = value;
    }


    bool ServerDetails::get_IsOfficialServer() {
        return ServerName == config.officialServerName;
    }

    bool ServerDetails::MatchesApiUrl(StringW apiUrl)  {
        if (apiUrl == ApiUrl) 
            // Exact match
            return true;

        if (System::String::IsNullOrEmpty(apiUrl))
            return false;

        // Loose match
        try {
            auto urlOurs = System::Uri::New_ctor(ApiUrl);
            auto urlTheirs = System::Uri::New_ctor(apiUrl);
            
            return urlOurs->Host == urlTheirs->Host &&
                urlOurs->Port == urlTheirs->Port;
        }
        catch (const std::exception& e) {
            return false;
        }
    }

    StringW ServerDetails::ToString() {
        return ServerName;
    }
}