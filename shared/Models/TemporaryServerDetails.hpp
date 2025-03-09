#pragma once
// #include "beatsaber-hook/shared/utils/gc-alloc.hpp"

#include "Models/ServerDetails.hpp"

// #include "System/Uri.hpp"

// DECLARE_CLASS_CUSTOM(BeatTogether::Models, TemporaryServerDetails, BeatTogether::Models::ServerDetails,
//     DECLARE_CTOR(_ctor, StringW graphApiUrl, StringW statusUrl = StringW());
// )

namespace BeatTogether::Models {
    /// @brief Represents the details of a server.
    struct TemporaryServerDetails : public ServerDetails {
        
        TemporaryServerDetails(std::string graphApiUrl, std::optional<std::string> statusUrl = std::nullopt);
    };
}