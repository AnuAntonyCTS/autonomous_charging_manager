# Autonomous Charging Manager (ROS 2)

ROS 2 package for autonomous battery monitoring, navigation to charging station, docking, charging, and mission resumption for the Lynx M20 quadruped robot.
This package implements an autonomous charging workflow without human intervention.

---

## Overview

The Autonomous Charging Manager monitors the robot battery state during operation.
When the battery level reaches the configured low battery threshold:

1. Patrol mission is interrupted.
2. Robot navigates to the charging point.
3. Robot performs close-range docking.
4. Charging is started.
5. Battery level is monitored.
6. Charging stops after reaching the required battery level.
7. Patrol mission resumes automatically.

The current implementation supports simulation mode and is prepared for integration with the real Lynx M20 robot.

---

## System Architecture

```
                 +----------------------+
                 | Battery Simulator    |
                 | /BATTERY_DATA        |
                 +----------+-----------+
                            |
                            v

                 +----------------------+
                 | Battery Monitor      |
                 |                      |
                 +----------+-----------+
                            |
                            v

              +---------------------------+
              | Autonomous Charging       |
              | Manager State Machine     |
              +-------------+-------------+
                            |
        +-------------------+-------------------+
        |                                       |
        v                                       v

+-------------------+                +-------------------+
| Robot Adapter     |                | Charging Client   |
|                   |                | UDP Protocol      |
+---------+---------+                +-------------------+

          |
          v

+-------------------+
| Simulation Robot  |
| Adapter           |
+-------------------+

          OR

+-------------------+
| M20 Robot Adapter |
+-------------------+

```

---

# Features

## Battery Monitoring

- Subscribes to ROS 2 battery topic:

```
/BATTERY_DATA
```

Message:

```
drdds/msg/BatteryData
```

Battery level is monitored continuously.

---

## Autonomous Charging Workflow

Implemented state machine:

```
PATROLLING

      |
      | Battery <= low threshold
      v

LOW_BATTERY

      |
      v

NAVIGATING_TO_CHARGING_POINT

      |
      v

ARRIVED_NEAR_DOCK

      |
      v

REFLECTOR_DOCKING

      |
      v

CHARGING

      |
      | Battery >= resume threshold
      v

RESUMING

      |
      v

PATROLLING

```

---

# Package Structure

```
autonomous_charging_manager

├── include
│
│── autonomous_charging_manager
│
│   ├── core
│   │   ├── autonomous_charging_manager.hpp
│   │   ├── battery_monitor.hpp
│   │   ├── robot_adapter.hpp
│   │   └── robot_factory.hpp
│   │
│   ├── network
│   │   ├── charging_client.hpp
│   │   ├── inspection_protocol.hpp
│   │   └── udp_client.hpp
│   │
│   └── robot
│       ├── m20_robot_adapter.hpp
│       ├── navigation_client.hpp
│       └── simulation_robot_adapter.hpp
│
│
├── src
│
│   ├── core
│   │   ├── autonomous_charging_manager.cpp
│   │   ├── battery_monitor.cpp
│   │   └── robot_factory.cpp
│   │
│   ├── network
│   │   ├── charging_client.cpp
│   │   ├── inspection_protocol.cpp
│   │   └── udp_client.cpp
│   │
│   ├── robot
│   │   ├── m20_robot_adapter.cpp
│   │   ├── navigation_client.cpp
│   │   └── simulation_robot_adapter.cpp
│   │
│   ├── battery_simulator.cpp
│   └── main.cpp
│
├── CMakeLists.txt
├── package.xml
└── README.md

```

---

# Dependencies

Tested with:

- ROS 2 Humble
- Ubuntu 22.04
- C++17

Required ROS packages:

```
rclcpp
std_msgs
std_srvs
drdds
```

---

# Build Instructions

Create workspace:

```bash
mkdir -p ~/m20_ws/src
cd ~/m20_ws/src
```

Clone or place the package:

```
autonomous_charging_manager
```

Build:

```bash
cd ~/m20_ws
colcon build --packages-select autonomous_charging_manager
```

Source workspace:

```bash
source install/setup.bash
```

---

# Running the Simulation

## Terminal 1: Start Battery Simulator

```bash
ros2 run autonomous_charging_manager battery_simulator_node
```

Example output:

```
[INFO] Battery Simulator Started

Publishing battery: 95%
Publishing battery: 90%
Publishing battery: 85%
...
```

---

## Terminal 2: Start Autonomous Charging Manager

```bash
ros2 run autonomous_charging_manager autonomous_charging_manager_node
```

Example:

```
Autonomous Charging Manager Started

Creating Simulation Robot Adapter

Patrolling. Battery level: 20%

State transition:
PATROLLING -> LOW_BATTERY

Low battery detected

Navigation to charging point started

State transition:
LOW_BATTERY -> NAVIGATING_TO_CHARGING_POINT

Reached charging point

State transition:
NAVIGATING_TO_CHARGING_POINT
-> ARRIVED_NEAR_DOCK

Robot arrived near dock.
Starting reflector docking.

Docking successful

Charging started

State transition:
REFLECTOR_DOCKING -> CHARGING

Battery charged.
Stopping charge.

Patrol mission resumed

State transition:
RESUMING -> PATROLLING

```

---

# Robot Abstraction Layer

The package uses a robot adapter interface:

```
RobotAdapter
       |
       |
       +---------------------+
       |                     |
       v                     v

SimulationRobotAdapter   M20RobotAdapter

```

This allows development and testing without physical hardware.

---

# Current Simulation Capabilities

Implemented:

Battery monitoring
Low battery detection 
Navigation simulation 
Charging dock approach
Reflector docking simulation
Charging simulation
Automatic charging stop 
Patrol mission resume
State machine validation 

---

# Real Robot Integration Plan

When the Lynx M20 robot becomes available:

Replace:

```
SimulationRobotAdapter
```

with:

```
M20RobotAdapter
```

Required integrations:

- Real navigation API
- Real docking point coordinates
- Reflector localization interface
- Charging command through inspection protocol
- Real battery DDS data

The state machine logic remains unchanged.

---

# Future Improvements

- Connect real Lynx M20 navigation system
- Add persistent patrol mission storage
- Add charging failure recovery
- Add communication timeout handling
- Add emergency battery protection mode
- Add multiple charging station support

---

# License

To be updated.
