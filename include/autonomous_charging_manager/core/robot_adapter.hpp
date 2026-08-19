#pragma once

class RobotAdapter
{
public:

    virtual ~RobotAdapter() = default;

    // Battery
    virtual int getBatteryLevel() = 0;

    // Patrol / mission control
    virtual bool startPatrol() = 0;
    virtual bool stopPatrol() = 0;
    virtual bool resumePatrol() = 0;

    // Long-range navigation to charging area
    virtual bool navigateToChargingPoint(
        double x,
        double y,
        double yaw) = 0;

    virtual bool isNavigationComplete() = 0;

    // Close-range docking
    virtual bool isDockingComplete() = 0;

    // Charging
    virtual bool startCharging() = 0;
    virtual bool stopCharging() = 0;
    virtual bool isCharging() = 0;

    // Error state
    virtual bool hasError() = 0;
};
