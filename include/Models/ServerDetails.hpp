#pragma once

#include "BeatTogether.hpp"

extern const std::string OfficialServerName;

DECLARE_JSON_CLASS(ServerDetails,
                    NAMED_VALUE(std::string, serverName, "ServerName");
                    NAMED_VALUE(std::string, hostName, "HostName");
                    NAMED_VALUE_DEFAULT(int, port, 2328, "Port");
                    NAMED_VALUE(std::string, statusURI, "StatusUri");
                    NAMED_VALUE_DEFAULT(int, maxPartySize, 5, "MaxPartySize");
public:
    bool isTemporary = false;
    bool isOfficial = false;
    ServerDetails(std::string _serverName, std::string _hostName, std::string _statusURI, int _maxPartySize = 5, int _port = 2328) : ServerDetails(_serverName) {
        this->serverName = std::move(_serverName);
        this->hostName =  std::move(_hostName);
        this->statusURI =  std::move(_statusURI);
        this->port = _port;
        this->maxPartySize = _maxPartySize;
    };
    explicit ServerDetails(bool createTemporary, std::string _hostName, int _port) : ServerDetails(_hostName) {
        this->isTemporary = createTemporary;
        if(this->isTemporary) {
            this->serverName = _hostName;
            this->hostName = _hostName;
            this->port = _port;
            this->statusURI = "";
            this->maxPartySize = isOfficial ? 5 : 128;
        }
    };
    explicit ServerDetails(std::string _serverName) {
        if(_serverName == OfficialServerName) {
            this->isOfficial = true;
        }
    }
    ServerDetails() {};
)