#include <chrono>
#include <rclcpp/rclcpp.hpp>

#include "autonomous_charging_manager/core/autonomous_charging_manager.hpp"


int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);


    auto node = std::make_shared<rclcpp::Node>(
        "autonomous_charging_manager_node"
    );


    auto charging_manager =
        std::make_shared<
            autonomous_charging_manager::AutonomousChargingManager
        >(node);



    RCLCPP_INFO(
        node->get_logger(),
        "Autonomous Charging Manager Node Started"
    );


    /*
     * Run state machine periodically
     *
     * 100 ms = 10 Hz update rate
     *
     */
    auto timer = node->create_wall_timer(
        std::chrono::milliseconds(100),
        [&]()
        {
            charging_manager->update();
        }
    );


    rclcpp::spin(node);


    rclcpp::shutdown();

    return 0;
}
