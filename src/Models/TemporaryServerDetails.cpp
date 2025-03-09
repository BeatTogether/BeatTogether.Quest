#include "Models/TemporaryServerDetails.hpp"
#include "System/Uri.hpp"

// DEFINE_TYPE(BeatTogether::Models, TemporaryServerDetails);

namespace BeatTogether::Models {
    // void TemporaryServerDetails::_ctor(
    TemporaryServerDetails::TemporaryServerDetails(
        std::string graphApiUrl, 
        std::optional<std::string> statusUrl
    ) 
    {
        try {
            // TODO: Maybe find a better way to parse the URL
            auto urlParsed = System::Uri::New_ctor(graphApiUrl);

            ServerName = std::string(urlParsed->Host);
            HostName = std::string(urlParsed->Host);
        }
        catch(const std::exception& e) {
            ServerName = std::string(graphApiUrl);
            HostName = std::string(graphApiUrl);
        }

        ApiUrl = graphApiUrl;
        StatusUrl = statusUrl.has_value() ? statusUrl.value() : graphApiUrl;
        MaxPartySize = IsOfficial ? 5 : 128;
        DisableSSL = true;
    }
}