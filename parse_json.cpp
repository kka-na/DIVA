#include "Make_json.h"
#include "Index_and_timestamp.h"

int main(){
   
    Make_json mj;

    mj.sensor(2);

    mj.log(0);

    mj.frame();

    mj.scene();

    mj.frame_data();

    mj.gps_data();

    return 0;
}