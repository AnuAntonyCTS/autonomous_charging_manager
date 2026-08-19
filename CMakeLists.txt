cmake_minimum_required(VERSION 3.8)
project(autonomous_charging_manager)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# -------------------------------
# Dependencies
# -------------------------------
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(drdds REQUIRED)
find_package(std_srvs REQUIRED)



# -------------------------------
# Executable
# -------------------------------

add_executable(battery_simulator_node
  src/battery_simulator.cpp
)

add_executable(udp_test_receiver
    src/udp_test_receiver.cpp
)

add_executable(autonomous_charging_manager_node

    src/main.cpp

    # Core
    src/core/autonomous_charging_manager.cpp
    src/core/robot_factory.cpp
    src/core/battery_monitor.cpp
    src/core/charge_status_monitor.cpp

    # Robot abstraction
    src/robot/m20_robot_adapter.cpp
    src/robot/simulation_robot_adapter.cpp
    src/robot/navigation_client.cpp

    # Network
    src/network/charging_client.cpp
    src/network/udp_client.cpp
    src/network/inspection_protocol.cpp

    
)

# -------------------------------
# Include directories
# -------------------------------
target_include_directories(
    autonomous_charging_manager_node
    PUBLIC
    include
)

target_include_directories(
    battery_simulator_node
    PUBLIC
    include
)

ament_target_dependencies(
    autonomous_charging_manager_node
    rclcpp
    std_msgs
    drdds
    std_srvs
)

ament_target_dependencies(
  battery_simulator_node
  rclcpp
  drdds
  std_msgs
  std_srvs
)


# -------------------------------
# Install
# -------------------------------
install(TARGETS
  autonomous_charging_manager_node
  battery_simulator_node
  udp_test_receiver
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
  DESTINATION include/
)

install(
  DIRECTORY
    launch
    config
  DESTINATION share/${PROJECT_NAME}
)

# -------------------------------
# Testing
# -------------------------------
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
