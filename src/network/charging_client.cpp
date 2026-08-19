#include "autonomous_charging_manager/network/charging_client.hpp"

ChargingClient::ChargingClient(
    rclcpp::Node::SharedPtr node,
    const std::string& robot_ip,
    int port,
    bool simulation)
:
node_(node),
simulation_(simulation),
udp_client_(robot_ip, port)
{

    if(simulation_)
    {
        start_client_ =
            node_->create_client<std_srvs::srv::Trigger>(
                "/start_charging"
            );


        stop_client_ =
            node_->create_client<std_srvs::srv::Trigger>(
                "/stop_charging"
            );
    }
}

bool ChargingClient::connect()
{
    return udp_client_.connect();
}

bool ChargingClient::sendHeartbeat()
{
    auto packet = protocol_.buildHeartbeatPacket();

    bool success = udp_client_.sendPacket(packet);

    if (!success)
    {
        RCLCPP_WARN(
            node_->get_logger(),
            "Failed to send heartbeat.");
    }

    return success;
}

bool ChargingClient::startCharging()
{

    if (simulation_)
{
    if (!start_client_->wait_for_service(std::chrono::seconds(2)))
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Start charging service unavailable");

        return false;
    }

    auto request =
        std::make_shared<std_srvs::srv::Trigger::Request>();

    start_client_->async_send_request(request);

    RCLCPP_INFO(
        node_->get_logger(),
        "Start charging request sent.");

    return true;
}
    auto packet = protocol_.buildChargePacket(1);

    bool success = udp_client_.sendPacket(packet);

    if (success)
    {
    	RCLCPP_INFO(
        	node_->get_logger(),
        	"Start charging command sent.");
	}
    else
    {
    	RCLCPP_ERROR(
       	 node_->get_logger(),
       	 "Failed to send start charging command.");
    }

    return success;
}

bool ChargingClient::stopCharging()
{
    if (simulation_)
{
    if (!stop_client_->wait_for_service(std::chrono::seconds(2)))
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Stop charging service unavailable");

        return false;
    }

    auto request =
        std::make_shared<std_srvs::srv::Trigger::Request>();

    stop_client_->async_send_request(request);

    RCLCPP_INFO(
        node_->get_logger(),
        "Stop charging request sent.");

    return true;
}
    auto packet = protocol_.buildChargePacket(0);

    bool success = udp_client_.sendPacket(packet);

    if (success)
    {
        RCLCPP_INFO(
            node_->get_logger(),
            "Stop charging command sent.");
    }
    else
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Failed to send stop charging command.");
    }

    return success;
}

bool ChargingClient::clearChargingState()
{
    auto packet = protocol_.buildChargePacket(2);
    return udp_client_.sendPacket(packet);
}

bool ChargingClient::sendNavigationGoal(
    double x,
    double y,
    double yaw)
{

    auto packet =
        protocol_.buildNavigationPacket(
            x,
            y,
            yaw);


    bool success =
        udp_client_.sendPacket(packet);


    if(success)
    {
        RCLCPP_INFO(
            node_->get_logger(),
            "Navigation goal sent: x=%.2f y=%.2f yaw=%.2f",
            x,
            y,
            yaw);
    }
    else
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Failed to send navigation goal");
    }


    return success;
}
