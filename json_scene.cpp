#include <jsoncpp/json/json.h>

Json::Value Scenes;
void scene()
{
	Json::Value a_scene;

	for (size_t i = 0; i < 2; i++)
	{
		a_scene["scene_token"] = generate_token();
		a_scene["log_token"] = Logs[0]["log_token"];
		a_scene["nbr_samples"] = 200;
		a_scene["first_frame_token"] = Frames[20 * i]["frame_token"];
		a_scene["last_frame_token"] = Frames[20 * (i + 1) - 1]["frame_token"];

		Scenes.append(a_scene);
		a_scene.clear();
	}

	Json::StyledWriter writer;
	string str = writer.write(Scenes);

	ofstream out("[DIVA_scenes]//json//scene.json");
	out << str;
	out.close();
}