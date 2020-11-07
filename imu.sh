#! /bin/bash
sudo chmod 777 /dev/ttyACM1
cd Documents
echo "Logging IMU..."
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/share/c++-mscl
g++ -I/usr/share/c++-mscl/source -I/usr/share/c++-mscl/Boost/include imu.cpp -o imu -L/usr/share/c++-mscl -lmscl -lstdc++ -std=c++11 -lpthread
./imu
