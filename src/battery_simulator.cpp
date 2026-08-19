#include "std_srvs/srv/trigger.hpp"
#include <functional>
#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "drdds/msg/battery_data.hpp"


using namespace std::chrono_literals;


class BatterySimulator : public rclcpp::Node
{

enum class Mode
{
    DISCHARGING,
    CHARGING
};

Mode mode_;

public:

    BatterySimulator()
    : Node("battery_simulator"),
      mode_(Mode::DISCHARGING),
      battery_level_(100)
    {

        declare_parameter("initial_battery", 100);
	declare_parameter("battery_step", 5);
	declare_parameter("publish_period", 5.0);

	battery_level_ = get_parameter("initial_battery").as_int();

	battery_step_ = get_parameter("battery_step").as_int();

	publish_period_ = get_parameter("publish_period").as_double();
        
        publisher_ =
            this->create_publisher<drdds::msg::BatteryData>(
                "/BATTERY_DATA",
                10
            );


        timer_ =
    	this->create_wall_timer(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::duration<double>(publish_period_)),
        std::bind(
            &BatterySimulator::publishBattery,
            this));
            
        start_service_ =
            this->create_service<std_srvs::srv::Trigger>(
                "start_charging",
                std::bind(
                    &BatterySimulator::startChargingService,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));

        stop_service_ =
            this->create_service<std_srvs::srv::Trigger>(
                "stop_charging",
                std::bind(
                    &BatterySimulator::stopChargingService,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));
                
        drain_service_ =
    	    this->create_service<std_srvs::srv::Trigger>(
        	"/drain_battery",
        	std::bind(
            	    &BatterySimulator::drainBattery,
            	    this,
            	    std::placeholders::_1,
                    std::placeholders::_2));


        RCLCPP_INFO(
            this->get_logger(),
            "Battery Simulator Started"
        );
    }
    
    void startChargingService(
    	const std::shared_ptr<std_srvs::srv::Trigger::Request>,
    	std::shared_ptr<std_srvs::srv::Trigger::Response> response)
    {
    	mode_ = Mode::CHARGING;

    	response->success = true;
    	response->message = "Charging mode enabled";

    	RCLCPP_INFO(this->get_logger(), "Charging mode enabled");
    }

    void stopChargingService(
    	const std::shared_ptr<std_srvs::srv::Trigger::Request>,
    	std::shared_ptr<std_srvs::srv::Trigger::Response> response)
    {
    	mode_ = Mode::DISCHARGING;

    	response->success = true;
    	response->message = "Discharging mode enabled";

    	RCLCPP_INFO(this->get_logger(), "Discharging mode enabled");
    }
    
    void drainBattery(
    	const std_srvs::srv::Trigger::Request::SharedPtr,
    	std_srvs::srv::Trigger::Response::SharedPtr response)
	{
    		battery_level_ = 10;

    		response->success = true;
    		response->message = "Battery drained to 10%";

    		RCLCPP_INFO(
        		this->get_logger(),
        		"Battery manually drained to 10%%"
    					);
	}


private:

    void publishBattery()
    {

        drdds::msg::BatteryData msg;


        drdds::msg::BatteryDataValue battery;


        


        if(mode_ == Mode::DISCHARGING)
		{
    		if(battery_level_ > 10)
        	battery_level_ -= battery_step_;
		}

	else if(mode_ == Mode::CHARGING)
		{
    		if(battery_level_ < 100)
        	battery_level_ += battery_step_;
		}
		
       battery.battery_level = battery_level_;


        msg.data.push_back(battery);


        publisher_->publish(msg);


        RCLCPP_INFO(
            this->get_logger(),
            "Publishing battery: %d%%",
            battery_level_
        );
    }



private:

    rclcpp::Publisher<drdds::msg::BatteryData>::SharedPtr publisher_;
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr start_service_;
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr stop_service_;
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr drain_service_;
    

    rclcpp::TimerBase::SharedPtr timer_;

    int battery_level_;
    int battery_step_;

    double publish_period_;
   
};



int main(int argc, char **argv)
{

    rclcpp::init(argc, argv);


    rclcpp::spin(
        std::make_shared<BatterySimulator>()
    );


    rclcpp::shutdown();

    return 0;
}
