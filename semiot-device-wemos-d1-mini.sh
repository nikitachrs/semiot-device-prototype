#!/bin/bash
make -f semiot-device.mk
while true; do
    read -p "Upload?" yn
    case $yn in
        [Yy]* ) make -f semiot-device.mk upload; break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done
