// #include "Models/TemporaryServerConfig.hpp"
// #include "System/Uri.hpp"
// #include "logging.hpp"

// namespace BeatTogether::Models {
//     TemporaryServerConfig::TemporaryServerConfig(
//         std::string graphApiUrl,
//         std::string statusUrl
//     ) 
//     {
//         try {
//             // TODO: Maybe find a better way to parse the URL, like using WebUtils
//             auto urlParsed = System::Uri::New_ctor<il2cpp_utils::CreationType::Manual>(graphApiUrl);

//             ServerName = urlParsed->Host;
//             HostName = urlParsed->Host;

//             gc_free_specific(urlParsed);
//         }
//         catch(const std::exception& e) {
//             ERROR("Failed to parse URL: {}", e.what());
//             ServerName = graphApiUrl;
//             HostName = graphApiUrl;
//         }

//         bool IsOfficial = ServerName == config.officialServerName;

//         this->graphUrl = graphApiUrl;
//         this->masterServerStatusUrl = System::String::IsNullOrEmpty(statusUrl) ? statusUrl : graphApiUrl;
//         this->maxPartySize = IsOfficial ? 5 : 128;
//         this->disableSSL = true;
//     }
// }