#pragma once

#include <rclcpp/rclcpp.hpp>
#include "drdds/msg/battery_data.hpp"

class BatteryMonitor
{
public:
    explicit BatteryMonitor(rclcpp::Node::SharedPtr node);

    int getBatteryLevel() const;

private:
    void batteryCallback(
        const drdds::msg::BatteryData::SharedPtr msg);

    rclcpp::Node::SharedPtr node_;

    rclcpp::Subscription<drdds::msg::BatteryData>::SharedPtr battery_sub_;

    int battery_level_ = -1;   // -1 means "not received yet"
};
