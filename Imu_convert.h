#ifndef Imu_convert_h
#define Imu_convert_h

#include "Index_and_timestamp.h"

class Imu_convert{
public:

	Index_and_timestamp iat;
	float gyro_x;
	float gyro_y;
	float gyro_z;
	float accel_x;
	float accel_y;
	float accel_z;
	float mag_x;
	float mag_y;
	float mag_z;
/*
	void Get_GAM(int imu_idx){ //get Zyro, Acceleration, Magnetic by index

		gyro_x = iat.imu_csv[imu_idx][1];
		gyro_y = iat.imu_csv[imu_idx][2];
		gyro_z = iat.imu_csv[imu_idx][3];
		accel_x = iat.imu_csv[imu_idx][4];
		accel_y = iat.imu_csv[imu_idx][5];
		accel_z = iat.imu_csv[imu_idx][6];
		mag_x = iat.imu_csv[imu_idx][7];
		mag_y = iat.imu_csv[imu_idx][8];
		mag_z = iat.imu_csv[imu_idx][9];

	}*/


};

#endif