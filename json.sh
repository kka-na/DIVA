#!/bin/bash

echo "Json Encoding"
cd Documents
g++ parse_json.cpp -ljsoncpp -o parse_json
./parse_json

