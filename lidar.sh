#! /bin/bash

cd Documents/ouster_example/ouster_viz/build
echo "Logging LiDAR..."
cmake .. && make && ./viz os1-991904000944.local 10.5.5.1

