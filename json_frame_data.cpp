#include "json_encoding.h"

Json::Value Frame_data;
void frame_data()
{
	Json::Value frame_datum;
	vector<vector<string>> sensor_csv;
	sensor_csv = read_csv("sensor_data\\csv\\lidar.csv");

	string token_prev = "";
	string token_curr = generate_token();
	cout << Sensors[0]["sensor_token"] << endl;
	for (size_t i = 0; i < sensor_csv.size(); i++)
	{
		string token_next = generate_token();

		frame_datum["frame_data_token"] = token_curr;
		frame_datum["frame_token"] = Frames[i]["frame_token"];
		frame_datum["ego_pose_token"] = generate_token(); //ego_pose json먼저 만들어야 하는데 오늘은 시간이 없어서 시험 이후에 만들게요
		frame_datum["can_token"] = generate_token();//얘도
		frame_datum["sensor_token"] = Sensors[0]["sensor_token"];
		frame_datum["filename"] = "filename";
		frame_datum["fileformat"] = "pcd";
		frame_datum["width"] = 0;
		frame_datum["height"] = 0;
		frame_datum["timestamp"] =generate_timestamp();
		if (frame_datum["timestamp"] == Frames[i]["timestamp"])
			frame_datum["is_key_frame"] = true;
		else
			frame_datum["is_key_frame"] = false;
		frame_datum["prev"] = token_prev;
		if (i == sensor_csv.size()-1)
			frame_datum["next"] = "";
		else
			frame_datum["next"] = token_next;

		Frame_data.append(frame_datum);
		frame_datum.clear();

		token_prev = token_curr;
		token_curr = token_next;
	}
	Json::StyledWriter writer;
	string str = writer.write(Frame_data);

	ofstream out("[DIVA_scenes]//json//frame_data.json");
	out << str;
	out.close();
}