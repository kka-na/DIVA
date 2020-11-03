#!/bin/bash

echo "Setting GPS"
sudo chmod 777 /dev/ttyACM0

sudo echo -e n'\xB5\x62\x06\x08\x06\x00\x64\x00\x01\x00\x01\x00\x7A\x12\xB5\x62\x06\x08\x00\x00\x0E\x30' > /dev/ttyACM0

cd Documents
echo "Logging GPS..."
g++ -o gps gps.cpp
./gps /dev/ttyACM0 9600

