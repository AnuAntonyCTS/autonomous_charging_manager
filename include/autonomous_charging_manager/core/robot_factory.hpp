#pragma once

#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"

#include "autonomous_charging_manager/core/robot_adapter.hpp"


namespace autonomous_charging_manager
{

class RobotFactory
{
public:

    static std::shared_ptr<RobotAdapter> create(
        rclcpp::Node::SharedPtr node,
        const std::string &robot_mode);

};
}

