#include "autonomous_charging_manager/core/charge_status_monitor.hpp"

ChargeStatusMonitor::ChargeStatusMonitor(
    rclcpp::Node::SharedPtr node)
    : node_(node),
      state_(-1),
      error_code_(0)
{
    charge_status_sub_ =
        node_->create_subscription<drdds::msg::StdStatus>(
            "/CHARGE_STATUS",
            10,
            std::bind(
                &ChargeStatusMonitor::chargeStatusCallback,
                this,
                std::placeholders::_1));

    RCLCPP_INFO(
        node_->get_logger(),
        "Charge Status Monitor initialized");
}

void ChargeStatusMonitor::chargeStatusCallback(
    const drdds::msg::StdStatus::SharedPtr msg)
{
    state_ = msg->state;
    error_code_ = msg->error_code;

    RCLCPP_INFO(
        node_->get_logger(),
        "Charge Status: state=%d error_code=0x%04X",
        state_,
        error_code_);
}

int ChargeStatusMonitor::getState() const
{
    return state_;
}

uint32_t ChargeStatusMonitor::getErrorCode() const
{
    return error_code_;
}

bool ChargeStatusMonitor::isCharging() const
{
    return state_ == 2;
}

bool ChargeStatusMonitor::isNavigatingToDock() const
{
    return state_ == 1;
}

bool ChargeStatusMonitor::hasError() const
{
    return state_ == 4 ||
           state_ == 5 ||
           error_code_ != 0;
}
