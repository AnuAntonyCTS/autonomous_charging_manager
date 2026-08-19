#include "autonomous_charging_manager/network/inspection_protocol.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

InspectionProtocol::InspectionProtocol()
: message_id_(0)
{
}

std::vector<uint8_t>
InspectionProtocol::buildNavigationPacket(
    double x,
    double y,
    double yaw)
{
    nlohmann::json packet;

    packet["PatrolDevice"]["Type"] = 1003;

    packet["PatrolDevice"]["Command"] = 1;

    packet["PatrolDevice"]["Time"] = getCurrentTime();

    packet["PatrolDevice"]["Items"] = {
        {"X", x},
        {"Y", y},
        {"Yaw", yaw},
        {"PointInfo", 3}
    };

    std::string body = packet.dump();

    auto header = buildHeader(body.size());

    header.insert(
        header.end(),
        body.begin(),
        body.end());

    return header;
}


std::vector<uint8_t>
InspectionProtocol::buildHeader(uint16_t data_length)
{
    std::vector<uint8_t> header(16, 0);

    header[0] = 0xEB;
    header[1] = 0x91;
    header[2] = 0xEB;
    header[3] = 0x90;

    header[4] = data_length & 0xFF;
    header[5] = (data_length >> 8) & 0xFF;

    header[6] = message_id_ & 0xFF;
    header[7] = (message_id_ >> 8) & 0xFF;

    header[8] = 0x01;

    message_id_++;

    return header;
}

std::string InspectionProtocol::getCurrentTime()
{
    std::time_t now = std::time(nullptr);

    std::tm* local_time = std::localtime(&now);

    std::ostringstream stream;

    stream << std::put_time(local_time,
                            "%Y-%m-%d %H:%M:%S");

    return stream.str();
}

std::vector<uint8_t>
InspectionProtocol::buildHeartbeatPacket()
{
    json packet;

    packet["PatrolDevice"]["Type"] = 100;
    packet["PatrolDevice"]["Command"] = 100;
    packet["PatrolDevice"]["Time"] = getCurrentTime();
    packet["PatrolDevice"]["Items"] = json::object();

    std::string json_string = packet.dump();

    std::vector<uint8_t> packet_bytes =
        buildHeader(static_cast<uint16_t>(json_string.size()));

    packet_bytes.insert(packet_bytes.end(),
                        json_string.begin(),
                        json_string.end());

    return packet_bytes;
}

std::vector<uint8_t>
InspectionProtocol::buildChargePacket(int charge_value)
{
    json packet;

    packet["PatrolDevice"]["Type"] = 2;
    packet["PatrolDevice"]["Command"] = 24;
    packet["PatrolDevice"]["Time"] = getCurrentTime();

    packet["PatrolDevice"]["Items"]["Charge"] = charge_value;

    std::string json_string = packet.dump();

    std::vector<uint8_t> packet_bytes =
        buildHeader(static_cast<uint16_t>(json_string.size()));

    packet_bytes.insert(packet_bytes.end(),
                        json_string.begin(),
                        json_string.end());

    return packet_bytes;
}
