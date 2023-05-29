#!/usr/bin/env bash

# insert device driver
sudo insmod ./memento.ko

# major device number is dynamically allocated
maj=$(cat /proc/devices|grep memento|cut -d' ' -f1)

# iterate over the number of minor devices and create a node
for min in $(seq 0 3); do
  sudo mknod /dev/memento${min} c ${maj} ${min}
done
