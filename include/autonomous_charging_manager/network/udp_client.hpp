#pragma once

#include <string>
#include <vector>

class UDPClient
{
public:

    UDPClient(const std::string& robot_ip,
              int port);

    ~UDPClient();

    bool connect();

    void disconnect();

    bool sendPacket(const std::vector<uint8_t>& packet);

    bool receivePacket(std::vector<uint8_t>& response);

private:

    std::string robot_ip_;
    int port_;

    int socket_fd_ = -1;
};
