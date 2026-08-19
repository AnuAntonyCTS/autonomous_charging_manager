#pragma once

#include "autonomous_charging_manager/core/robot_adapter.hpp"

#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/trigger.hpp"
#include "drdds/msg/battery_data.hpp"


class SimulationRobotAdapter : public RobotAdapter
{
public:

    explicit SimulationRobotAdapter(
        rclcpp::Node::SharedPtr node);

    // Battery
    int getBatteryLevel() override;

    // Patrol
    bool startPatrol() override;
    bool stopPatrol() override;
    bool resumePatrol() override;

    // Navigation
    bool navigateToChargingPoint(
        double x,
        double y,
        double yaw) override;

    bool isNavigationComplete() override;

    // Docking
    bool isDockingComplete() override;

    // Charging
    bool startCharging() override;
    bool stopCharging() override;
    bool isCharging() override;

    // Error
    bool hasError() override;

    void batteryCallback(
        const drdds::msg::BatteryData::SharedPtr msg);


private:

    rclcpp::Node::SharedPtr node_;

    // Battery
    int battery_level_;

    // Patrol
    bool patrol_active_;

    // Navigation
    rclcpp::Time navigation_start_time_;
    bool navigation_active_;
    bool navigation_complete_;

    // Charging
    bool charging_;

    // Docking
    bool docking_complete_;
    rclcpp::Time docking_start_time_;

    // Error
    bool error_;

    // Charging simulator services
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr
        start_charge_client_;

    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr
        stop_charge_client_;

    // Battery subscription
    rclcpp::Subscription<drdds::msg::BatteryData>::SharedPtr
        battery_sub_;
};
