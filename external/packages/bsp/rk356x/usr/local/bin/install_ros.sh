#!/bin/bash

#mirror_url=http://mirrors.ustc.edu.cn
mirror_url=https://repo.huaweicloud.com

if [[ -n $1 && $1 =~ ros1|ros2 ]]; then
	version=$1
else
	echo "usage: install_ros.sh ros1/ros2"
	exit
fi

release=$(lsb_release -cs)

if [[ $version == "ros1" && $release =~ focal ]]; then

	[[ -f /etc/apt/sources.list.d/ros-latest.list ]] && sudo rm /etc/apt/sources.list.d/ros-latest.list
	sudo sh -c "echo deb ${mirror_url}/ros/ubuntu $(lsb_release -sc) main > /etc/apt/sources.list.d/ros1.list"
	sudo apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
	sudo apt update
	sudo apt install -y ros-noetic-desktop-full

	sudo sh -c 'echo "source /opt/ros/noetic/setup.bash" >> /root/.bashrc'
	echo "source /opt/ros/noetic/setup.bash" >> /home/orangepi/.bashrc

	sudo apt install -y python3-rosdep python3-rosinstall python3-rosinstall-generator python3-wstool build-essential

	sudo sh -c 'echo "151.101.84.133 raw.githubusercontent.com" >> /etc/hosts'
	source /opt/ros/noetic/setup.bash
	sudo rosdep init
	rosdep update

	exit
fi

if [[ $version == "ros2" && $release =~ focal ]]; then

	sudo apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
	echo "deb [arch=$(dpkg --print-architecture)] ${mirror_url}/ros2/ubuntu $(source /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list

	sudo apt update
	sudo apt install -y ros-galactic-desktop
	sudo apt install -y ros-dev-tools

	sudo sh -c 'echo "source /opt/ros/galactic/setup.bash" >> /root/.bashrc'
	echo "source /opt/ros/galactic/setup.bash" >> /home/orangepi/.bashrc

	source /opt/ros/galactic/setup.bash
	ros2 -h

	exit

fi

if [[ $version == "ros2" && $release =~ jammy ]]; then

	sudo apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
	echo "deb [arch=$(dpkg --print-architecture)] ${mirror_url}/ros2/ubuntu $(source /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list

	sudo apt update
	sudo apt install -y ros-humble-desktop
	sudo apt install -y ros-dev-tools

	sudo sh -c 'echo "source /opt/ros/humble/setup.bash" >> /root/.bashrc'
	echo "source /opt/ros/humble/setup.bash" >> /home/orangepi/.bashrc

	source /opt/ros/humble/setup.bash
	ros2 -h

	exit

fi

echo "Unsupported System!"
