#include "autonomous_charging_manager/network/udp_client.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

UDPClient::UDPClient(const std::string& robot_ip,
                     int port)
    : robot_ip_(robot_ip),
      port_(port)
{
}

UDPClient::~UDPClient()
{
    disconnect();
}

bool UDPClient::connect()
{
    socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd_ < 0)
    {
        std::cerr << "Failed to create UDP socket." << std::endl;
        return false;
    }

    return true;
}

void UDPClient::disconnect()
{
    if (socket_fd_ >= 0)
    {
        close(socket_fd_);
        socket_fd_ = -1;
    }
}

bool UDPClient::sendPacket(const std::vector<uint8_t>& packet)
{
    if (socket_fd_ < 0)
    {
        return false;
    }

    sockaddr_in robot_addr{};

    robot_addr.sin_family = AF_INET;
    robot_addr.sin_port = htons(port_);

    if (inet_pton(AF_INET,
                  robot_ip_.c_str(),
                  &robot_addr.sin_addr) <= 0)
    {
        return false;
    }

    ssize_t bytes_sent =
        sendto(socket_fd_,
               packet.data(),
               packet.size(),
               0,
               reinterpret_cast<sockaddr*>(&robot_addr),
               sizeof(robot_addr));

    return bytes_sent == static_cast<ssize_t>(packet.size());
}

bool UDPClient::receivePacket(std::vector<uint8_t>& response)
{
    if (socket_fd_ < 0)
    {
        return false;
    }

    response.resize(4096);

    ssize_t bytes =
        recvfrom(socket_fd_,
                 response.data(),
                 response.size(),
                 0,
                 nullptr,
                 nullptr);

    if (bytes < 0)
    {
        return false;
    }

    response.resize(bytes);

    return true;
}
