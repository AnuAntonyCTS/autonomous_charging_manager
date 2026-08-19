#pragma once

#include <rclcpp/rclcpp.hpp>


class NavigationClient
{

public:

    NavigationClient(
        rclcpp::Node::SharedPtr node);


    bool goToPoint(
        double x,
        double y,
        double yaw);


    bool isComplete();


private:

    rclcpp::Node::SharedPtr node_;

    bool navigation_complete_;

    rclcpp::TimerBase::SharedPtr navigation_timer_;

};
