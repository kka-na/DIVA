
#include <opencv2/opencv.hpp>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgcodecs.hpp>

#include "Linux_keyboard.h"
#include "Timestamp.h"
#include <vector>


using namespace std; 
using namespace cv;

/* @ function main */
int main(){ 

    Linux_keyboard lk;
    Timestamp ts;

    Mat frame;
    char buf[256];
    VideoCapture cap(0);

    if( !cap.isOpened() ) return -1;

    string path = "/home/kanakim/Documents/CAM/i30_CAM_ts_"+ts.getMilliTime()+".txt";
    ofstream wirteFile;
    wirteFile.open(path.c_str());
    double fps = 30.0;
    int index = 0;

    cout<<"The CAM_ts.txt file saved to ["<<path<<"]\n";

    lk.init_keyboard();

    vector<int> Compression_params;
    Compression_params.push_back(IMWRITE_JPEG_QUALITY); //Select jpeg
         Compression_params.push_back(10);

    while(1){
        cap >> frame;
        //cv::imshow("WebCam Frame Capture", frame );

        string m_time = ts.getMilliTime();

        const char * now_time = ts.p_time();
        sprintf(buf,"/home/kanakim/Documents/CAM/JPG/i30_CAM_%s.jpg", now_time);
        string tmp = "\n";
        wirteFile.write(now_time, m_time.size());
        wirteFile.write(tmp.c_str(), tmp.size());

        imwrite(buf, frame,Compression_params);

        index++;

        if(lk._kbhit()){
            int ch = lk._getch();
            if(ch>=10) break;
        }
        waitKey(27);     
    }
    wirteFile.close();
    lk.close_keyboard();
    
    return 0;
}