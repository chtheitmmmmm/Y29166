#!/usr/bin/env bash

# check if memento devices already exist and remove
if [ `ls /dev | grep memento | wc -l` -gt 0 ]; then
	sudo rm /dev/memento[0-9]*
	echo "Removed existing memento devices"
fi
# check if memento module is inserted and remove
if [ `lsmod | grep memento | wc -l` -gt 0 ]; then
	sudo rmmod memento
	echo "Removed existing memento modules"
fi

# make module/device driver, exist if failure (compilation error)
make || { echo "Build aborted: Make error"; exit 1; }

# minor device number
if [ -n "$1" ]; then
	if [ $1 -lt 1 ]; then
		echo "minor number must not be less then 1"
		exit 1
	elif [ $1 -gt 10 ]; then
		echo "minor number must not be greater then 10"
		exit 1
	else
	   num=$1
	fi
else
	num=$(($RANDOM % 9 + 1))
fi

echo "Will create ${num} devices"

# insert module
sudo insmod ./memento.ko num=${num}

# major device number is dynamically allocated
maj=$(cat /proc/devices | grep memento| cut -d ' ' -f1)

# iterate over the number of minor devices and create a node
for min in $(seq 0 $((${num} - 1))); do
	sudo mknod /dev/memento$((${min})) c ${maj} $((${min})) -m 666
done

