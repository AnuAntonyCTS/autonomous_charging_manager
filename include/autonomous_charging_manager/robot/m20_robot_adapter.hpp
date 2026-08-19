#pragma once

#include <rclcpp/rclcpp.hpp>

#include "autonomous_charging_manager/core/robot_adapter.hpp"
#include "autonomous_charging_manager/core/battery_monitor.hpp"
#include "autonomous_charging_manager/core/charge_status_monitor.hpp"
#include "autonomous_charging_manager/network/charging_client.hpp"

namespace autonomous_charging_manager
{

class M20RobotAdapter : public RobotAdapter
{

public:

    explicit M20RobotAdapter(
        rclcpp::Node::SharedPtr node);

    // Battery
    int getBatteryLevel() override;

    // Patrol
    bool startPatrol() override;
    bool stopPatrol() override;
    bool resumePatrol() override;

    // Long-range navigation
    bool navigateToChargingPoint(
        double x,
        double y,
        double yaw) override;

    bool isNavigationComplete() override;

    // Charging
    bool startCharging() override;
    bool stopCharging() override;
    bool isCharging() override;

    // Docking
    bool isDockingComplete() override;

    // Error
    bool hasError() override;


private:

    rclcpp::Node::SharedPtr node_;

    std::unique_ptr<BatteryMonitor> battery_monitor_;
    
    std::unique_ptr<ChargeStatusMonitor> charge_status_monitor_;

    std::unique_ptr<ChargingClient> charging_client_;

    /*
     * Navigation state.
     *
     * This is NOT simulated here.
     * It will be updated when the deployed
     * navigation system is integrated.
     */
    bool navigation_complete_;
};

}
