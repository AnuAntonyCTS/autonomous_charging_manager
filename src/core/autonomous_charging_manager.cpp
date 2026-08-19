#include "autonomous_charging_manager/core/autonomous_charging_manager.hpp"
#include "autonomous_charging_manager/core/robot_factory.hpp"

namespace autonomous_charging_manager
{

AutonomousChargingManager::AutonomousChargingManager(
        rclcpp::Node::SharedPtr node)

: node_(node),
  current_state_(State::PATROLLING),
  low_battery_threshold_(20),
  resume_battery_threshold_(80),
  navigation_complete_(false),
  patrol_started_(false)

{
    node_->declare_parameter<int>(
        "low_battery_threshold",
        20);

    node_->declare_parameter<int>(
        "resume_battery_threshold",
        80);

    node_->declare_parameter<double>(
        "charging_x",
        5.0);

    node_->declare_parameter<double>(
        "charging_y",
        3.0);

    node_->declare_parameter<double>(
        "charging_yaw",
        0.0);

    node_->declare_parameter<std::string>(
        "robot_mode",
        "simulation");

    node_->declare_parameter<int>(
        "navigation_timeout_seconds",
        60);

    node_->declare_parameter<int>(
        "max_navigation_retries",
        3);


    low_battery_threshold_ =
        node_->get_parameter(
            "low_battery_threshold").as_int();


    resume_battery_threshold_ =
        node_->get_parameter(
            "resume_battery_threshold").as_int();


    charging_x_ =
        node_->get_parameter(
            "charging_x").as_double();


    charging_y_ =
        node_->get_parameter(
            "charging_y").as_double();


    charging_yaw_ =
        node_->get_parameter(
            "charging_yaw").as_double();


    navigation_timeout_ =
        rclcpp::Duration(
            node_->get_parameter(
                "navigation_timeout_seconds").as_int(),
            0);


    max_navigation_retries_ =
        node_->get_parameter(
            "max_navigation_retries").as_int();


    navigation_retry_count_ = 0;


    std::string robot_mode =
        node_->get_parameter(
            "robot_mode").as_string();


    RCLCPP_INFO(
        node_->get_logger(),
        "Autonomous Charging Manager Started");


    robot_ =
        RobotFactory::create(
            node_,
            robot_mode);

}

void AutonomousChargingManager::update()
{

    switch(current_state_)
    {

        case State::PATROLLING:
            handlePatrolling();
            break;

        case State::LOW_BATTERY:
            handleLowBattery();
            break;

        case State::NAVIGATING_TO_CHARGING_POINT:
    	    handleNavigationToChargingPoint();
            break;


	case State::ARRIVED_NEAR_DOCK:
    	    handleArrivedNearDock();
    	    break;


	case State::REFLECTOR_DOCKING:
    	    handleReflectorDocking();
            break;

        case State::CHARGING:
            handleCharging();
            break;

        case State::RESUMING:
            handleResuming();
            break;
            
        case State::ERROR:
    	    handleError();
            break;

    }
}

void AutonomousChargingManager::transitionTo(State new_state)
{
    if(current_state_ == new_state)
    {
        return;
    }

    RCLCPP_INFO(
        node_->get_logger(),
        "State transition: %s -> %s",
        stateToString(current_state_),
        stateToString(new_state));

    current_state_ = new_state;
}

void AutonomousChargingManager::handlePatrolling()
{
    if (!patrol_started_)
{
    if (!robot_->startPatrol())
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Failed to start patrol");

        transitionTo(State::ERROR);

        return;
    }

    patrol_started_ = true;

    RCLCPP_INFO(
        node_->get_logger(),
        "Patrol started");
}
    int battery = robot_->getBatteryLevel();

    if (battery < 0)
    {
        RCLCPP_WARN_THROTTLE(
            node_->get_logger(),
            *node_->get_clock(),
            5000,
            "Waiting for battery data...");

        return;
    }

    RCLCPP_INFO_THROTTLE(
        node_->get_logger(),
        *node_->get_clock(),
        5000,
        "Patrolling. Battery level: %d%%",
        battery);

    if (battery <= low_battery_threshold_)
    {
        transitionTo(State::LOW_BATTERY);
    }
}

void AutonomousChargingManager::handleLowBattery()
{
    RCLCPP_INFO(
        node_->get_logger(),
        "Low battery detected");


    /*
     * First stop the current patrol mission.
     *
     * The RobotAdapter decides how patrol is stopped.
     * In simulation this is simulated.
     * On the real M20 this will eventually connect
     * to the deployed patrol/mission system.
     */

    if (!robot_->stopPatrol())
{
    RCLCPP_ERROR(
        node_->get_logger(),
        "Failed to stop patrol");

    transitionTo(State::ERROR);

    return;
}




    RCLCPP_INFO(
        node_->get_logger(),
        "Patrol stopped");


    /*
     * Now request navigation to the charging point.
     *
     * IMPORTANT:
     * The charging manager does NOT implement navigation.
     * It only requests it through RobotAdapter.
     */

    navigation_start_time_ =
        node_->now();


    if (robot_->navigateToChargingPoint(
            charging_x_,
            charging_y_,
            charging_yaw_))
    {
        RCLCPP_INFO(
            node_->get_logger(),
            "Navigation to charging point started");

        transitionTo(
            State::NAVIGATING_TO_CHARGING_POINT);
    }
    else
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Failed to start navigation");

        transitionTo(
            State::ERROR);
    }
}  

void AutonomousChargingManager::handleNavigationToChargingPoint()
{

    RCLCPP_INFO_THROTTLE(
        node_->get_logger(),
        *node_->get_clock(),
        3000,
        "Navigating to charging point"
    );
    
    auto elapsed =
    node_->now() - navigation_start_time_;

if(elapsed > navigation_timeout_)
{
    navigation_retry_count_++;

    if(navigation_retry_count_ <= max_navigation_retries_)
    {
        RCLCPP_WARN(
            node_->get_logger(),
            "Navigation timed out. Retrying (%d/%d)...",
            navigation_retry_count_,
            max_navigation_retries_);

        navigation_start_time_ = node_->now();

        robot_->navigateToChargingPoint(
            charging_x_,
            charging_y_,
            charging_yaw_);

        return;
    }

    RCLCPP_ERROR(
        node_->get_logger(),
        "Navigation failed after %d retries.",
        max_navigation_retries_);

    transitionTo(State::ERROR);

    return;
}


    if(robot_->isNavigationComplete())
    {

        RCLCPP_INFO(
            node_->get_logger(),
            "Reached charging point"
        );
        
        navigation_retry_count_ = 0;


        navigation_complete_ = false;


        transitionTo(
            State::ARRIVED_NEAR_DOCK);
    }

}

void AutonomousChargingManager::handleArrivedNearDock()
{
    RCLCPP_INFO(
        node_->get_logger(),
        "Robot arrived near dock. Starting reflector docking.");

    transitionTo(State::REFLECTOR_DOCKING);
}

void AutonomousChargingManager::handleReflectorDocking()
{

    if(robot_->isDockingComplete())
    {
        RCLCPP_INFO(
            node_->get_logger(),
            "Docking successful");


        if(robot_->startCharging())
        {
            RCLCPP_INFO(
                node_->get_logger(),
                "Charging started");

            transitionTo(State::CHARGING);
        }
        else
        {
            RCLCPP_ERROR(
                node_->get_logger(),
                "Failed to start charging");

            transitionTo(State::ERROR);
        }

    }
    else
    {
        RCLCPP_INFO_THROTTLE(
            node_->get_logger(),
            *node_->get_clock(),
            3000,
            "Waiting for reflector docking");
    }

}

void AutonomousChargingManager::handleCharging()
{

    int battery = robot_->getBatteryLevel();

if (battery < 0)
{
    RCLCPP_WARN(
        node_->get_logger(),
        "Waiting for battery data...");
    return;
}


    RCLCPP_INFO_THROTTLE(
        node_->get_logger(),
        *node_->get_clock(),
        5000,
        "Charging... Battery: %d%%",
        battery);


    if(battery >= resume_battery_threshold_)
    {

        RCLCPP_INFO(
            node_->get_logger(),
            "Battery charged. Stopping charge."
        );


        if (robot_->stopCharging())
	{
    RCLCPP_INFO(
        node_->get_logger(),
        "Charging stopped"
    );

    transitionTo(State::RESUMING);
	}
	else
	{
    RCLCPP_ERROR(
        node_->get_logger(),
        "Failed to stop charging"
    );

    transitionTo(State::ERROR);
	}
    }

}

void AutonomousChargingManager::handleResuming()
{
    RCLCPP_INFO(
        node_->get_logger(),
        "Resuming patrol");

    if (robot_->resumePatrol())
    {
        patrol_started_ = true;

        RCLCPP_INFO(
            node_->get_logger(),
            "Patrol resumed successfully");

        transitionTo(State::PATROLLING);
    }
    else
    {
        RCLCPP_ERROR(
            node_->get_logger(),
            "Failed to resume patrol");

        transitionTo(State::ERROR);
    }
}

void AutonomousChargingManager::handleError()
{
    RCLCPP_ERROR_THROTTLE(
        node_->get_logger(),
        *node_->get_clock(),
        5000,
        "Charging manager is in ERROR state."
    );
}

const char* AutonomousChargingManager::stateToString(State state) const
{
    switch(state)
    {
        case State::PATROLLING:
            return "PATROLLING";

        case State::LOW_BATTERY:
            return "LOW_BATTERY";

        case State::NAVIGATING_TO_CHARGING_POINT:
            return "NAVIGATING_TO_CHARGING_POINT";

        case State::ARRIVED_NEAR_DOCK:
            return "ARRIVED_NEAR_DOCK";

        case State::REFLECTOR_DOCKING:
            return "REFLECTOR_DOCKING";

        case State::CHARGING:
            return "CHARGING";

        case State::RESUMING:
            return "RESUMING";

        case State::ERROR:
            return "ERROR";
    }

    return "UNKNOWN";
}
}
