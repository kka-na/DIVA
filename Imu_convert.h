#ifndef Imu_convert_h
#define Imu_convert_h

#include "Index_and_timestamp.h"

class Imu_convert{
public:

	Index_and_timestamp iat;

	string gyro_x;
	string gyro_y;
	string gyro_z;
	string accel_x;
	string accel_y;
	string accel_z;
	string mag_x;
	string mag_y;
	string mag_z;

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

	}


};

#endif