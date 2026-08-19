#include "autonomous_charging_manager/robot/m20_robot_adapter.hpp"

namespace autonomous_charging_manager
{

M20RobotAdapter::M20RobotAdapter(
    rclcpp::Node::SharedPtr node)
:
node_(node),
navigation_complete_(false)
{
    RCLCPP_INFO(
        node_->get_logger(),
        "M20 Robot Adapter initialized");

    /*
     * Battery feedback
     */
    battery_monitor_ =
        std::make_unique<BatteryMonitor>(
            node_);\
            
    /*
 * Autonomous charging status feedback
 */
    charge_status_monitor_ =
    std::make_unique<ChargeStatusMonitor>(
        node_);

    /*
     * Charging communication.
     *
     * The actual robot IP / simulation mode will
     * be configured when the real M20 communication
     * endpoint is available.
     */
    charging_client_ =
        std::make_unique<ChargingClient>(
            node_,
            "127.0.0.1",
            30000,
            true);

    if (!charging_client_->connect())
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Failed to initialize charging client");
    }
    else
    {
        RCLCPP_INFO(
            node_->get_logger(),
            "Charging client connected");
    }
}


/*
 * --------------------------------------------------------------------------
 * Battery
 * --------------------------------------------------------------------------
 */

int M20RobotAdapter::getBatteryLevel()
{
    return battery_monitor_->getBatteryLevel();
}


/*
 * --------------------------------------------------------------------------
 * Patrol
 * --------------------------------------------------------------------------
 *
 * The deployed patrol/mission API is outside the SDK currently available
 * to us.
 *
 * Therefore these methods intentionally do NOT pretend that patrol control
 * is available.
 */

bool M20RobotAdapter::startPatrol()
{
    RCLCPP_WARN(
        node_->get_logger(),
        "M20 patrol start requested, but deployed patrol API is not "
        "available in the current SDK");

    return false;
}


bool M20RobotAdapter::stopPatrol()
{
    RCLCPP_WARN(
        node_->get_logger(),
        "M20 patrol stop requested, but deployed patrol API is not "
        "available in the current SDK");

    return false;
}


bool M20RobotAdapter::resumePatrol()
{
    RCLCPP_WARN(
        node_->get_logger(),
        "M20 patrol resume requested, but deployed patrol API is not "
        "available in the current SDK");

    return false;
}


/*
 * --------------------------------------------------------------------------
 * Long-range navigation
 * --------------------------------------------------------------------------
 */

bool M20RobotAdapter::navigateToChargingPoint(
    double x,
    double y,
    double yaw)
{
    RCLCPP_WARN(
        node_->get_logger(),
        "M20 navigation requested: "
        "x=%.2f y=%.2f yaw=%.2f",
        x,
        y,
        yaw);

    /*
     * IMPORTANT:
     *
     * The current SDK does not expose the deployed M20 navigation command.
     *
     * Therefore we must NOT send a fabricated navigation packet and we must
     * NOT simulate completion here.
     *
     * This method is the integration boundary for the deployed navigation
     * system that will be available with the real robot/system.
     */

    navigation_complete_ = false;

    RCLCPP_WARN(
        node_->get_logger(),
        "M20 navigation interface is not available yet");

    return false;
}


bool M20RobotAdapter::isNavigationComplete()
{
    return navigation_complete_;
}


/*
 * --------------------------------------------------------------------------
 * Charging
 * --------------------------------------------------------------------------
 */

bool M20RobotAdapter::startCharging()
{
    bool result =
        charging_client_->startCharging();

    if (result)
    {
        RCLCPP_INFO(
            node_->get_logger(),
            "Charging started through ChargingClient");
    }
    else
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Failed to start charging");
    }

    return result;
}


bool M20RobotAdapter::stopCharging()
{
    bool result =
        charging_client_->stopCharging();

    if (result)
    {
        RCLCPP_INFO(
            node_->get_logger(),
            "Charging stopped through ChargingClient");
    }
    else
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Failed to stop charging");
    }

    return result;
}


bool M20RobotAdapter::isCharging()
{
    if (!charge_status_monitor_)
    {
        return false;
    }

    return charge_status_monitor_->isCharging();
}


/*
 * --------------------------------------------------------------------------
 * Docking
 * --------------------------------------------------------------------------
 */

bool M20RobotAdapter::isDockingComplete()
{
    /*
     * Reflector-based docking feedback will be integrated here when the
     * deployed docking/localization interface is available.
     *
     * Do NOT report success on the real M20 just to advance the state machine.
     */

    return false;
}


/*
 * --------------------------------------------------------------------------
 * Error
 * --------------------------------------------------------------------------
 */

bool M20RobotAdapter::hasError()
{
    if (!charge_status_monitor_)
    {
        return false;
    }

    return charge_status_monitor_->hasError();
}

}
