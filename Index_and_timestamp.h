#ifndef Index_and_timestamp_h
#define Index_and_timestamp_h

#include "Read_files.h"


#define is_GPS 1
#define is_CAM  2
#define is_LiDAR 3
#define is_IMU 4
#define  number_of_sensors 2


class Index_and_timestamp{
public:

	Read_files rf;
	
	int latest_sensor;
    int earliest_sensor;
    int gps_last;
    int s_ts = 0;



	int get_hhmmsssss(string str){ return stoi(str.substr(8,9));}


    //Get Files!! 
	vector<vector<string>> gps_csv = rf.read_csv(rf.get_path(is_GPS));
    vector<string> cam_txt = rf.read_txt(rf.get_path(is_CAM));



    int find_cam_idx_by_ts(int now, string comp_timestamp){
        int cam_start_idx = 0;
        for(int i=now; i<cam_txt.size(); i++){
            int difference = get_hhmmsssss(comp_timestamp) - get_hhmmsssss(cam_txt[i]);
            if(difference<0) difference = - difference;
            if(difference <= 40){
                cam_start_idx = i;
                break;
            }
        }
        return cam_start_idx;

    }

    int find_gps_idx_by_ts(int now, string comp_timestamp){
        int gps_start_idx = 0;
        for(int i=now; i<gps_csv.size(); i++){
            int difference = get_hhmmsssss(comp_timestamp) - get_hhmmsssss(gps_csv[i][0]);
            if(difference<0) difference = -difference;
            if(difference <= 100){
                gps_start_idx = i;
                break;
            }
        }
        return gps_start_idx;
    }



    int find_cam_start_idx(){
        int cam_start_idx = 0;
        for(int i=0; i<cam_txt.size(); i++){
            int difference = s_ts - get_hhmmsssss(cam_txt[i]);
            if(difference < 0 && -difference <= 40){
                cam_start_idx = i;
                break;
            }
        }
        return cam_start_idx;

    }

    int find_gps_start_idx(){
        int gps_start_idx = 0;
        for(int i=0; i<gps_csv.size(); i++){
            int difference = s_ts - get_hhmmsssss(gps_csv[i][0]);
            if(difference < 0 && -difference <= 100){
                gps_start_idx = i;
                break;
            }
        }
        return gps_start_idx;
    }


    string find_latest_started(){

        int first_timestamps[number_of_sensors+1];
        latest_sensor = is_GPS;

        first_timestamps[is_GPS] =get_hhmmsssss(gps_csv[0][0]);
        first_timestamps[is_CAM] = get_hhmmsssss(cam_txt[0]);

        int max = first_timestamps[latest_sensor];

        for(int i = latest_sensor+1; i <= number_of_sensors; i++){
            if(max < first_timestamps[i]){
                max = first_timestamps[i];
                latest_sensor = i;
            }
        }

        switch(latest_sensor){
            case is_GPS:
            return gps_csv[0][0];

            case is_CAM:
            latest_sensor = is_CAM;
            return cam_txt[0];
        }

    }

    string find_earliest_ended(){
        int  last_timestamps[number_of_sensors+1];
        earliest_sensor = is_GPS;

        last_timestamps[is_GPS] = get_hhmmsssss(gps_csv[gps_csv.size()-1][0]);
        last_timestamps[is_CAM] = get_hhmmsssss(cam_txt[cam_txt.size()-1]);

        int min = last_timestamps[earliest_sensor];
        for(int i=earliest_sensor+1; i<=number_of_sensors; i++){
            if(min>last_timestamps[i]){
                min = last_timestamps[i];
                earliest_sensor = i;
            }
        }

        switch(earliest_sensor){
            case is_GPS:
            return gps_csv[gps_csv.size()-1][0];

            case is_CAM:
            return cam_txt[cam_txt.size()-1];
        }



    }

    int number_of_frames(){
        int number_of_frames = 0;
        int gps_start =  find_gps_start_idx();
        string last_timestamp = find_earliest_ended();
        gps_last = find_gps_idx_by_ts((gps_csv.size())/2, last_timestamp);
        if(gps_last == gps_csv.size()-1) gps_last = gps_last-1;

        number_of_frames = gps_last - gps_start;

        return number_of_frames;
    }


    int* get_start_indexes(){
       static int idxes[number_of_sensors+1];
       string start_timestamp = find_latest_started();
       s_ts = get_hhmmsssss(start_timestamp);

       idxes[is_GPS] = find_gps_start_idx();
       idxes[is_CAM] = find_cam_start_idx();

    return idxes;

}


bool txt_sensor_is_key_frame(int isSensor, int target_idx, int gps_idx){
    vector<string> txts;
    bool is_key_frame = true;

    int dev = 0;

    if(isSensor == is_CAM){
        txts = cam_txt;
        dev = 15;
    }else if(isSensor == is_LiDAR){
        //txts = lidar_txt
        //dev = 20;
    }

    int comp = get_hhmmsssss(gps_csv[gps_idx][0]) - get_hhmmsssss(txts[target_idx]);

    if(comp < 0) comp = -comp;
    if(comp > dev) is_key_frame = false;
    //cout<<"["<<gps_idx<<"] - ["<<target_idx<<"] "<<get_hhmmsssss(gps_csv[gps_idx][0])<<" - "<< get_hhmmsssss(txts[target_idx])<<"="<<comp<<endl;


    return is_key_frame;
}

/*
bool csv_sensor_is_key_frame(int isSensor, int target_idx, int gps_idx){
    vector<vector<string>> csvs;
    bool is_key_frame = true;

    int dev = 0;

    if(isSensor == is_IMU){
        csvs = imu_csv;
        dev = 50;
    }

    int comp = get_hhmmsssss(gps_csv[gps_idx][0])- get_hhmmsssss(csvs[target_idx][0]);

    if(comp <0) comp = -comp;
    if(comp > dev) is_key_frame = false;

    return is_key_frame;
}
*/

string get_gps_timestamp(int gps_idx){
    return gps_csv[gps_idx][0];
}
string get_cam_timestamp(int cam_idx){
    return cam_txt[cam_idx];
}
/*
string get_lidar_timestamp(int lidar_idx){
    return lidar_txt[lidar_idx];
}
string get_imu_timestamp(int imu_idx){
    return imu_txt[imu_idx];
}
*/
};

#endif
