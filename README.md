# DIVA

Description of BASCH files

//gps.sh
#!/bin/bash

echo "Setting GPS"
sudo chmod 777 /dev/ttyACM0

sudo echo -e n'\xB5\x62\x06\x08\x06\x00\x64\x00\x01\x00\x01\x00\x7A\x12\xB5\x62$

cd Documents
echo "Logging GPS..."
g++ -o gps gps.cpp
./gps /dev/ttyACM0 9600


//cam.sh
#!/bin/bash

echo "Setting CAM"
cd Documents
echo "Logging CAM..."
g++ -std=c++11 cam.cpp -o cam `pkg-config --cflags --libs opencv4`
./cam


//test.sh
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

//json.sh
#!/bin/bash

echo "Json Encoding"
cd Documents
g++ parse_json.cpp -ljsoncpp -o parse_json
./parse_json



