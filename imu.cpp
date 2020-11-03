#include "mscl/mscl.h"
#include <iostream>
#include <vector>
#include <pthread.h>
#include <cstring>
#include "Linux_keyboard.h"
#include "Timestamp.h"

using namespace std;

class IMUdata {
public:
    float yaw, pitch, roll, magx, magy, magz, accelx, accely, accelz;
};

int main(){
	Linux_keyboard lk;
	Timestamp ts;

	mscl::Connection connection = mscl::Connection::Serial("/dev/ttyACM1", 115200);
	mscl::InertialNode node(connection);

	char name[256] = "/home/kanakim/Documents/IMU/i30_IMU_";
    string str = ts.getMilliTime();
    vector<char> writable(str.begin(), str.end());
    writable.push_back('\0');
    char* ptr = &writable[0];
    strcat(name, ptr);

	FILE* fp;
    char* filename = strcat(name, ".csv");
    fp = fopen(filename, "w+");

    lk.init_keyboard();

	while(true)
	{
    	mscl::MipDataPackets packets = node.getDataPackets(500);

    	IMUdata temp;

    	for(mscl::MipDataPacket packet : packets)
    	{
    		int count = 0;	

        	packet.descriptorSet();
    	    mscl::MipDataPoints points = packet.data();

        	for(mscl::MipDataPoint dataPoint : points)
        	{
        		const char * cName = dataPoint.channelName().c_str();
        		const char* mtime = ts.p_time();

  	      		if(strcmp("scaledAccelX", cName) == 0) {
 	       			//cout << mtime << endl;
        			count = 9;
        			temp.accelx = dataPoint.as_float();
  	      		}

  	      		if(count == 8) temp.accely = dataPoint.as_float();
  	      		if(count == 7) temp.accelz = dataPoint.as_float();
  	      		if(count == 6) temp.yaw = dataPoint.as_float();
  	      		if(count == 5) temp.pitch = dataPoint.as_float();
  	      		if(count == 4) temp.roll = dataPoint.as_float();
  	      		if(count == 3) temp.magx = dataPoint.as_float();
  	      		if(count == 2) temp.magy = dataPoint.as_float();
  	      		if(count == 1) temp.magz = dataPoint.as_float();

				//if(0 < count && count <= 9) cout << dataPoint.channelName() << " : " << dataPoint.as_float() << endl;

				count--;
        
				if(count == 0) {
					fprintf(fp, "%s,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", mtime, temp.yaw, temp.pitch, temp.roll, temp.magx, temp.magy, temp.magz, temp.accelx, temp.accely, temp.accelz);
					break;
				}
        	}

        	if(count == 0) break;

   		}

   		usleep(25000);

   		if(lk._kbhit()){
   			int ch = lk._getch();
   			if(ch >= 10) break;
   		}
	}

	lk.close_keyboard();
	fclose(fp);

	return 0;
}