#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

class InspectionProtocol
{
public:

    InspectionProtocol();
    std::string getCurrentTime();

    std::vector<uint8_t> buildHeartbeatPacket();

    std::vector<uint8_t> buildChargePacket(int charge_value);

    std::vector<uint8_t> buildNavigationPacket(
        double x,
        double y,
        double yaw);

private:

    std::vector<uint8_t> buildHeader(uint16_t data_length);

    uint16_t message_id_;
};
