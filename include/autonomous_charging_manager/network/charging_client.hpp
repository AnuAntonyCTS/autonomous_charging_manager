#pragma once

#include "autonomous_charging_manager/network/udp_client.hpp"
#include "autonomous_charging_manager/network/inspection_protocol.hpp"

#include <rclcpp/rclcpp.hpp>
#include "std_srvs/srv/trigger.hpp"


class ChargingClient
{
public:

    ChargingClient(
        rclcpp::Node::SharedPtr node,
        const std::string& robot_ip,
        int port,
        bool simulation = true
    );


    bool connect();

    bool sendHeartbeat();

    bool startCharging();

    bool stopCharging();

    bool clearChargingState();
    
    bool sendNavigationGoal(
    double x,
    double y,
    double yaw);


private:

    rclcpp::Node::SharedPtr node_;

    bool simulation_;

    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr start_client_;

    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr stop_client_;


    UDPClient udp_client_;

    InspectionProtocol protocol_;
};
