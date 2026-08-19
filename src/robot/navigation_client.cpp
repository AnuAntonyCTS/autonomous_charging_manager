#include "autonomous_charging_manager/robot/navigation_client.hpp"


NavigationClient::NavigationClient(
    rclcpp::Node::SharedPtr node)
:
node_(node),
navigation_complete_(false)
{

}


bool NavigationClient::goToPoint(
    double x,
    double y,
    double yaw)
{

    RCLCPP_INFO(
        node_->get_logger(),
        "Navigation request x=%.2f y=%.2f yaw=%.2f",
        x,y,yaw);


    navigation_complete_ = false;


    navigation_timer_ =
        node_->create_wall_timer(
            std::chrono::seconds(5),
            [this]()
            {

                navigation_complete_ = true;


                RCLCPP_INFO(
                    node_->get_logger(),
                    "Navigation completed");


                navigation_timer_->cancel();

            });


    return true;
}



bool NavigationClient::isComplete()
{
    return navigation_complete_;
}
