#ifndef Gps_convert_h
#define Gps_convert_h

#include "Index_and_timestamp.h"
#include "jsoncpp/json/json.h"

class Gps_convert{
public:
	Index_and_timestamp iat;
	string latitude;
	string longitude;
	string altitude;
	string null = "NULL";
	//altitude iat.gps_csv[gps_idx][5]; // altitude

	void Get_LLA(int gps_idx){

		long double raw_latitude = stold(iat.gps_csv[gps_idx][1]); // latitude
		latitude = to_string(Convert_to_dd(raw_latitude));

		long double raw_longitude = stold(iat.gps_csv[gps_idx][3]); // longitude
		longitude = to_string(Convert_to_dd(raw_longitude));
		
		altitude = iat.gps_csv[gps_idx][5]; 

	}

	long double Convert_to_dd(long double raw){

		int dd = (int)(raw/100);
		long double ddddd = raw - (long double)(dd*100);
		ddddd /= 60;
	
		return (long double)dd+ddddd;
	}

};

#endif