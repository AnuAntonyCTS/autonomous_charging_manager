#include "autonomous_charging_manager/core/battery_monitor.hpp"

BatteryMonitor::BatteryMonitor(rclcpp::Node::SharedPtr node)
    : node_(node),
      battery_level_(-1)
{
    battery_sub_ =
        node_->create_subscription<drdds::msg::BatteryData>(
            "/BATTERY_DATA",
            10,
            std::bind(&BatteryMonitor::batteryCallback,
                      this,
                      std::placeholders::_1));
}

void BatteryMonitor::batteryCallback(
    const drdds::msg::BatteryData::SharedPtr msg)
{
    if (!msg->data.empty())
    {
        battery_level_ = msg->data[0].battery_level;

        RCLCPP_INFO(
            node_->get_logger(),
            "Battery Level: %d%%",
            battery_level_
        );
    }
}

int BatteryMonitor::getBatteryLevel() const
{
    return battery_level_;
}
