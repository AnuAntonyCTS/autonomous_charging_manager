#include "autonomous_charging_manager/robot/simulation_robot_adapter.hpp"

#include <chrono>
#include <functional>


SimulationRobotAdapter::SimulationRobotAdapter(
    rclcpp::Node::SharedPtr node)
:
node_(node),
battery_level_(-1),
patrol_active_(false),
navigation_active_(false),
navigation_complete_(false),
charging_(false),
docking_complete_(false),
error_(false)
{
    /*
     * Battery feedback
     */

    battery_sub_ =
        node_->create_subscription<drdds::msg::BatteryData>(
            "/BATTERY_DATA",
            10,
            std::bind(
                &SimulationRobotAdapter::batteryCallback,
                this,
                std::placeholders::_1
            )
        );


    /*
     * Charging simulation services
     */

    start_charge_client_ =
        node_->create_client<std_srvs::srv::Trigger>(
            "/start_charging");

    stop_charge_client_ =
        node_->create_client<std_srvs::srv::Trigger>(
            "/stop_charging");


    RCLCPP_INFO(
        node_->get_logger(),
        "Simulation Robot Adapter Started");
}


/*
 * --------------------------------------------------------------------------
 * Battery
 * --------------------------------------------------------------------------
 */

int SimulationRobotAdapter::getBatteryLevel()
{
    return battery_level_;
}


void SimulationRobotAdapter::batteryCallback(
    const drdds::msg::BatteryData::SharedPtr msg)
{
    if (!msg->data.empty())
    {
        battery_level_ =
            msg->data[0].battery_level;

        RCLCPP_INFO_THROTTLE(
            node_->get_logger(),
            *node_->get_clock(),
            5000,
            "Simulation battery: %d%%",
            battery_level_);
    }
}


/*
 * --------------------------------------------------------------------------
 * Patrol
 * --------------------------------------------------------------------------
 */

bool SimulationRobotAdapter::startPatrol()
{
    if (patrol_active_)
    {
        return true;
    }

    patrol_active_ = true;

    RCLCPP_INFO(
        node_->get_logger(),
        "Simulation patrol started");

    return true;
}


bool SimulationRobotAdapter::stopPatrol()
{
    if (!patrol_active_)
    {
        return true;
    }

    patrol_active_ = false;

    RCLCPP_INFO(
        node_->get_logger(),
        "Simulation patrol stopped");

    return true;
}


bool SimulationRobotAdapter::resumePatrol()
{
    patrol_active_ = true;

    RCLCPP_INFO(
        node_->get_logger(),
        "Simulation patrol resumed");

    return true;
}


/*
 * --------------------------------------------------------------------------
 * Navigation
 * --------------------------------------------------------------------------
 */

bool SimulationRobotAdapter::navigateToChargingPoint(
    double x,
    double y,
    double yaw)
{
    RCLCPP_INFO(
        node_->get_logger(),
        "Simulation navigation started: "
        "x=%.2f y=%.2f yaw=%.2f",
        x,
        y,
        yaw);


    navigation_active_ = true;
    navigation_complete_ = false;

    docking_complete_ = false;
    docking_start_time_ = rclcpp::Time(0, 0, node_->get_clock()->get_clock_type());

    navigation_start_time_ = node_->now();

    return true;
}


bool SimulationRobotAdapter::isNavigationComplete()
{
    if (navigation_active_)
    {
        auto elapsed =
            node_->now() - navigation_start_time_;


        if (elapsed.seconds() >= 5.0)
        {
            navigation_complete_ = true;
            navigation_active_ = false;

            RCLCPP_INFO(
                node_->get_logger(),
                "Simulation navigation completed");
        }
    }

    return navigation_complete_;
}


/*
 * --------------------------------------------------------------------------
 * Docking
 * --------------------------------------------------------------------------
 */

bool SimulationRobotAdapter::isDockingComplete()
{
    /*
     * Simulate reflector-based docking.
     *
     * The real M20 Pro will eventually replace this
     * with actual docking/localization feedback.
     */

    if (docking_complete_)
    {
        return true;
    }

    if (docking_start_time_.nanoseconds() == 0)
    {
        docking_start_time_ = node_->now();

        RCLCPP_INFO(
            node_->get_logger(),
            "Simulation reflector docking started");

        return false;
    }

    auto elapsed =
        node_->now() - docking_start_time_;

    if (elapsed.seconds() >= 5.0)
    {
        docking_complete_ = true;

        RCLCPP_INFO(
            node_->get_logger(),
            "Simulation reflector docking completed");
    }

    return docking_complete_;
}


/*
 * --------------------------------------------------------------------------
 * Charging
 * --------------------------------------------------------------------------
 */

bool SimulationRobotAdapter::startCharging()
{
    if (!start_charge_client_->wait_for_service(
            std::chrono::seconds(1)))
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Charging service not available");

        return false;
    }


    auto request =
        std::make_shared<std_srvs::srv::Trigger::Request>();


    start_charge_client_->async_send_request(request);

    charging_ = true;

    RCLCPP_INFO(
        node_->get_logger(),
        "Simulation charging started");

    return true;
}


bool SimulationRobotAdapter::stopCharging()
{
    if (!stop_charge_client_->wait_for_service(
            std::chrono::seconds(1)))
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Stop charging service not available");

        return false;
    }


    auto request =
        std::make_shared<std_srvs::srv::Trigger::Request>();


    stop_charge_client_->async_send_request(request);

    charging_ = false;

    RCLCPP_INFO(
        node_->get_logger(),
        "Simulation charging stopped");

    return true;
}


bool SimulationRobotAdapter::isCharging()
{
    return charging_;
}


/*
 * --------------------------------------------------------------------------
 * Error
 * --------------------------------------------------------------------------
 */

bool SimulationRobotAdapter::hasError()
{
    return error_;
}
