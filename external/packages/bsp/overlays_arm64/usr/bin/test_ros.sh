#!/bin/bash

if [[ -f /opt/ros/noetic/setup.bash ]]; then

	source /opt/ros/noetic/setup.bash
	roscore &

	sleep 5

	rosrun turtlesim turtlesim_node &
	rosrun turtlesim turtle_teleop_key

fi

if [[ -f /opt/ros/galactic/setup.bash ]]; then

	source /opt/ros/galactic/setup.bash
	ros2 run demo_nodes_cpp talker &
	ros2 run demo_nodes_py listener

fi

if [[ -f /opt/ros/humble/setup.bash ]]; then

	source /opt/ros/humble/setup.bash
	ros2 run demo_nodes_cpp talker &
	ros2 run demo_nodes_py listener

fi
