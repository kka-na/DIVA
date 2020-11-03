#!/bin/bash

sh cam.sh 01 &
sh gps.sh 02 &

echo -n "Enter [ESC] to finished programs"
read -s -n1 key
case $key in
	$'\e')
		ps -ef | grep cam | awk '{print $2}' | xargs kill -9
		ps -ef | grep gps | awk '{print $2}' | xargs kill -9
		echo "Finished"
		exit
		;;
esac


wait

