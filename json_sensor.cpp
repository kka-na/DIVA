#include "json_encoding.h"

Json::Value Sensors;
void sensor()
{
	Json::Value sensor;

	string token = generate_token();
	sensor["sensor_token"] = token;
	sensor["Channel"] = "LiDAR_TOP";
	sensor["modality"] = "LiDAR";

	Sensors.append(sensor);
	sensor.clear();

	
	Json::StyledWriter writer;
	string str = writer.write(Sensors);

	ofstream out("[DIVA_scenes]//json//sensor.json");
	out << str;
	out.close();
}