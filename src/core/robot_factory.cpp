#include "autonomous_charging_manager/core/robot_factory.hpp"

#include "autonomous_charging_manager/robot/m20_robot_adapter.hpp"
#include "autonomous_charging_manager/robot/simulation_robot_adapter.hpp"


namespace autonomous_charging_manager
{


std::shared_ptr<RobotAdapter>
RobotFactory::create(
    rclcpp::Node::SharedPtr node,
    const std::string &robot_mode)
{


    if(robot_mode == "m20")
    {

        RCLCPP_INFO(
            node->get_logger(),
            "Creating M20 Robot Adapter");


        return std::make_shared<M20RobotAdapter>(
            node);
    }


    else
    {

        RCLCPP_INFO(
            node->get_logger(),
            "Creating Simulation Robot Adapter");


        return std::make_shared<SimulationRobotAdapter>(
            node);
    }

}
}
