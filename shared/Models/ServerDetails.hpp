#pragma once

#include "multiplayer-core/shared/ServerConfig.hpp"

// #include "custom-types/shared/macros.hpp"

// /// @brief Represents the details of a server.
// DECLARE_CLASS_CODEGEN(BeatTogether::Models, ServerDetails, Il2CppObject,
//     // DECLARE_CTOR(_ctor, StringW serverName, StringW hostName, StringW apiUrl, StringW statusUrl, int maxPartySize, bool disableSSL);
//     DECLARE_DEFAULT_CTOR();

//     DECLARE_INSTANCE_FIELD(StringW, ServerName);
//     DECLARE_INSTANCE_FIELD(StringW, HostName);
//     DECLARE_INSTANCE_FIELD(StringW, ApiUrl);
//     DECLARE_INSTANCE_FIELD(StringW, StatusUrl);
//     DECLARE_INSTANCE_FIELD(int, MaxPartySize);
//     DECLARE_INSTANCE_FIELD(bool, DisableSSL);

//     DECLARE_INSTANCE_METHOD(ServerDetails*, Make, StringW serverName, StringW hostName, StringW apiUrl, StringW statusUrl, int maxPartySize, bool disableSSL);

//     DECLARE_OVERRIDE_METHOD_MATCH(StringW, ToString, &System::Object::ToString);

//     DECLARE_INSTANCE_METHOD(bool, get_IsOfficialServer);
//     DECLARE_INSTANCE_METHOD(bool, MatchesApiUrl, StringW apiUrl);

// public:
//     __declspec(property(get = get_IsOfficialServer)) bool IsOfficial;
// )



namespace BeatTogether::Models {
    struct ServerDetails : public MultiplayerCore::ServerConfig {
        const std::string OfficialServerName = "Official Servers";

        std::string get_ApiUrl();
        void set_ApiUrl(std::string value);
        std::string get_StatusUrl();
        void set_StatusUrl(std::string value);
        int get_MaxPartySize();
        void set_MaxPartySize(int value);
        bool get_DisableSSL();
        void set_DisableSSL(bool value);

        bool get_IsOfficialServer();

        std::string ServerName;
        std::string HostName;
        __declspec(property(get = get_ApiUrl, put = set_ApiUrl)) std::string ApiUrl;
        __declspec(property(get = get_StatusUrl, put = set_StatusUrl)) std::string StatusUrl;
        __declspec(property(get = get_MaxPartySize, put = set_MaxPartySize)) int MaxPartySize;
        __declspec(property(get = get_DisableSSL, put = set_DisableSSL)) bool DisableSSL;
        

        __declspec(property(get = get_IsOfficialServer)) bool IsOfficial;

        ServerDetails() = default;
        ServerDetails(std::string serverName, std::string hostName, std::string apiUrl, std::string statusUrl, int maxPartySize = 5, bool disableSSL = true);

        StringW ToString();

        bool MatchesApiUrl(StringW apiUrl);

        // bool MatchesApiUrl(StringW apiUrl) {
        //     if (apiUrl == ApiUrl) 
        //         // Exact match
        //         return true;

        //     // TODO: Implement loose match if we need it
        //     // if (apiUrl.empty())
        //     //     return false;

        //     // Loose match
        //     /* C# code:
        //     try
        //     {
        //         var urlOurs = new Uri(ApiUrl);
        //         var urlTheirs = new Uri(apiUrl);
                
        //         return urlOurs.Host == urlTheirs.Host &&
        //             urlOurs.Port == urlTheirs.Port;
        //     }
        //     catch (UriFormatException)
        //     {
        //         return false;
        //     }
        //     */
        //     return false;
        // }
    };
}