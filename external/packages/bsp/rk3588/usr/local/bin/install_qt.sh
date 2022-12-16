#!/bin/bash

release=$(lsb_release -cs)

sudo apt update
if [[ $release =~ focal|bionic|buster ]]; then
	sudo apt-get -y install qt5-default qttools5-dev-tools qtbase5-doc-html qt5-assistant qt5-doc
elif [[ $release =~ bullseye|jammy ]]; then
	sudo apt-get -y install qttools5-dev-tools qtbase5-doc-html qt5-assistant qt5-doc qt5-qmake qt5-qmake-bin
else
	echo "Unsupported system!"
	exit
fi

sudo apt-get -y install qtcreator qmlscene gdb qtdeclarative5-dev qtbase5-examples cmake

sudo chown orangepi:orangepi /usr/lib/aarch64-linux-gnu/qt5/examples -R

qmake -v
