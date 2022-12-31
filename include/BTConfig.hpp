#pragma once

#include "BeatTogether.hpp"

#include "config-utils/shared/config-utils.hpp"
#include <string>

#ifndef HOST_NAME
#error "Define HOST_NAME!"
#endif

#ifndef PORT
#error "Define PORT!"
#endif

#ifndef STATUS_URL
#error "Define STATUS_URL!"
#endif

DECLARE_CONFIG(BeatTogetherConfig,
     // Set defaults to compile time definitions.
     CONFIG_VALUE(hostName, std::string, "HOST_NAME", HOST_NAME);
     CONFIG_VALUE(port, int, "PORT", PORT);
     CONFIG_VALUE(status_url, std::string, "STATUS_URL", STATUS_URL);
)