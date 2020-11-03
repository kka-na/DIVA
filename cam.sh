#!/bin/bash

echo "Setting CAM"
cd Documents
echo "Logging CAM..."
g++ -std=c++11 cam.cpp -o cam `pkg-config --cflags --libs opencv4`
./cam

