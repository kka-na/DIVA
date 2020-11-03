#include "json_encoding.h"
Json::Value Logs;
void log()
{
	Json::Value log;
	string vehicles[2] = { "n008", "n004" };
	for (size_t i = 0; i < 3; i++)
	{
		string vehicle = vehicles[rand() % 2];
		string date = generate_time();

		log["token"] = generate_token();
		log["vehicle"] = vehicle;
		log["date_captured"] = date;
		log["logfile"] = generate_logfile(vehicle, date);
		log["location"] = "Incheon, South Korea";

		Logs.append(log);
		log.clear();
	}

	Json::StyledWriter writer;
	string str = writer.write(Logs);

	ofstream out("[DIVA_scenes]//json//log.json");
	out << str;
	out.close();
}