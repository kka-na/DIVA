#include "json_encoding.h"

Json::Value Frames;
void frame()
{
	Json::Value a_frame;

	string token_prev = "";
	string token_curr = generate_token();
	/* demo version */
	for (size_t i = 0; i < 16; i++)
	{
		string timestamp = generate_timestamp();
		string token_next = generate_token();

		a_frame["frame_token"] = token_curr;
		a_frame["timestamp"] = timestamp;
		a_frame["token_prev"] = token_prev;

		if (i == 19)
			a_frame["token_next"] = "";
		else
			a_frame["token_next"] = token_next;

		Frames.append(a_frame);
		a_frame.clear();

		token_prev = token_curr;
		token_curr = token_next;
	}

	Json::StyledWriter writer;
	string str = writer.write(Frames);

	ofstream out("[DIVa_frames]//json//frame.json");
	out << str;
	out.close();
}