from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():

    return LaunchDescription([

        Node(
            package="autonomous_charging_manager",
            executable="autonomous_charging_manager_node",
            name="autonomous_charging_manager_node",
            output="screen",
            parameters=[
                "config/charging_manager.yaml"
            ]
        )

    ])
