
#include "Linux_keyboard.h"
#include "Timestamp.h"
#include "Strtok_m.h"
#include "Can_serial.h"
#include "Speed_spec.h"

using namespace std;

Speed_spec speeds[] = {
	
	{"1200", B1200},
	{"2400", B2400},
	{"4800", B4800},
	{"9600", B9600},
	{"19200", B19200},
	{"38400", B38400},
	{"57600", B57600},
	{"115200", B115200},
	{NULL, 0}

};
	
int main(int argc, char *argv[]){

	Linux_keyboard lk;
	Timestamp ts;
	Strtok_m stm;
	Can_serial cs;

	int iDev = 0;
	int iRet = 0;

	sleep(3);

	const char *devicename = argv[1];
	
	int speed = B9600; //default
	iDev = open(devicename, O_RDWR | O_NOCTTY);

	if(0>iDev){
		perror(devicename);
		exit(-100);
	}

	if(argc > 2) {	
		Speed_spec *s;
		for(s = speeds; s->name; s++) {
			if(strcmp(s->name, argv[2]) == 0) { //same brate in speed_spec 
				speed = s->flag; //change spped value to user input
				//fprintf(stderr, "setting speed %s\n", s->name);
				break;
			}
		}
	}

	cs.init_serial(iDev,speed);

	char cBuff[255];
	char path[256];

	lk.init_keyboard();

		/*
		while(!file_made){

			int iFile = read(iDev, filename, 255);
			filename[iFile] = 0;

			char *start_time = strtok_f(filename, ',');

			if(strcmp(start_time,"$GNGGA") == 0){
				start_time = strtok_f(NULL, ',');
				start_time = utc_to_kst(start_time);
				sprintf(path, "/home/kanakim/Documents/test_csv/i30_GPS_%08s.csv", start_time);
				
				file_made = true;
			}

		}*/		
	const char * mtime = ts.p_time();

	sprintf(path, "/home/kanakim/Documents/GPS/i30_GPS_%s.csv", mtime);

	printf("The GPS file saved [%s]\n", path);

	FILE* fp = fopen(path, "w+");

	while(1){
		const char *nmea[50]={NULL, };

		iRet = read(iDev, cBuff, 255);
		cBuff[iRet] = 0;

		const char *ptr = stm.strtok_f(cBuff, ',');

		int cnt = 0;
		if(strcmp(ptr,"$GNGGA") == 0 ){
			while(ptr!=NULL){
				if(cnt == 1){
					nmea[1] = ts.p_time();
				}else{
					nmea[cnt] = ptr;
				}
				cnt++;
				ptr = stm.strtok_f(NULL, ',');
			}
			fprintf(fp, "%s,%s,%s,%s,%s,%s,%s\n",nmea[1], nmea[2], nmea[3], nmea[4], nmea[5], nmea[9], nmea[10]);
		}

		if(lk._kbhit()){
			int ch = lk._getch();
			if(ch>=10) break;
		}
	}

	lk.close_keyboard();
	fclose(fp);	

	cs.close_serial(iDev);

	return 0;
}

