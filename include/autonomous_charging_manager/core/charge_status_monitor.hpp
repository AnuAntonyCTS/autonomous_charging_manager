#pragma once

#include <rclcpp/rclcpp.hpp>
#include "drdds/msg/std_status.hpp"

class ChargeStatusMonitor
{
public:

    explicit ChargeStatusMonitor(
        rclcpp::Node::SharedPtr node);

    int getState() const;

    uint32_t getErrorCode() const;

    bool isCharging() const;

    bool isNavigatingToDock() const;

    bool hasError() const;

private:

    void chargeStatusCallback(
        const drdds::msg::StdStatus::SharedPtr msg);

    rclcpp::Node::SharedPtr node_;

    rclcpp::Subscription<drdds::msg::StdStatus>::SharedPtr
        charge_status_sub_;

    int state_ = -1;

    uint32_t error_code_ = 0;
};
