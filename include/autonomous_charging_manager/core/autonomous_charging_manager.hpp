#pragma once

#include <rclcpp/rclcpp.hpp>

#include "autonomous_charging_manager/core/robot_adapter.hpp"

#include "autonomous_charging_manager/core/robot_factory.hpp"

namespace autonomous_charging_manager
{

class AutonomousChargingManager
{
public:

    explicit AutonomousChargingManager(
        rclcpp::Node::SharedPtr node);


    void update();


private:

    enum class State
    {
	PATROLLING,

	LOW_BATTERY,

	NAVIGATING_TO_CHARGING_POINT,

	ARRIVED_NEAR_DOCK,

	REFLECTOR_DOCKING,

	CHARGING,

	RESUMING,

	ERROR
    };


    void transitionTo(State new_state);
    const char* stateToString(State state) const;


    void handlePatrolling();

    void handleLowBattery();

    void handleNavigationToChargingPoint();

    void handleArrivedNearDock();

    void handleReflectorDocking();

    void handleCharging();

    void handleResuming();
    
    void handleError();



private:

    rclcpp::Node::SharedPtr node_;

    State current_state_;

    int low_battery_threshold_;
    int resume_battery_threshold_;

    // Simulation variables

    rclcpp::Time navigation_start_time_;
    
    bool use_simulation_;

    bool navigation_complete_;
    
    bool patrol_started_;
    
    std::shared_ptr<RobotAdapter> robot_;
    
    rclcpp::Duration navigation_timeout_{60, 0};

    int navigation_retry_count_;

    int max_navigation_retries_;

    double charging_x_;
    double charging_y_;
    double charging_yaw_;
    
};
}
