#pragma once

#include "BeatTogether.hpp"

struct DnsEndpoint {
    std::string host;
    int port;

};

DECLARE_JSON_CLASS(ServerDetails,
                   NAMED_VALUE(std::string, serverName, "ServerName");
                           NAMED_VALUE(std::string, hostName, "HostName");
                           NAMED_VALUE_DEFAULT(int, port, 2328, "Port");
                           NAMED_VALUE(std::string, statusURI, "StatusUri");
                           NAMED_VALUE_DEFAULT(int, maxPartySize, 5, "MaxPartySize");
                           public:
                           DnsEndpoint getEndpoint();
)