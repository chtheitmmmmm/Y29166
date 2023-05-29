#!/usr/bin/env bash

# check if memento devices already exist and remove
if [ $(ls /dev | grep memento | wc -l) -gt 0 ]
then
    sudo rm /dev/memento[0-9]*
    echo "Removed existing memento devices"
fi
# check if memento module is inserted and remove
if [ $(lsmod | grep memento | wc -l) -gt 0 ]
then
    sudo rmmod memento
    echo "Removed existing memento module"
fi

# make module/device driver, exit if failure (compilation error)
make || { echo "Build aborted: Make error";  exit 1; }

# insert module
sudo insmod ./memento.ko

# major device number is dynamically allocated
maj=$(cat /proc/devices | grep memento | cut -d ' ' -f1)

# iterate over the number of minor devices and create a node
for min in $(seq 0 3); do
    sudo mknod /dev/memento${min} c ${maj} ${min} -m 666
done
