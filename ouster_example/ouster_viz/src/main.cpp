#include <unistd.h>
#include <atomic>
#include <cmath>
#include <csignal>
#include <cstdlib>
#include <chrono>

#include <Eigen/Eigen>
#include <fstream>
#include <istream>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "ouster/os1.h"
#include "ouster/os1_packet.h"
#include "ouster/os1_util.h"
#include "ouster/viz.h"//visualization
#include "ouster/Linux_keyboard.h"

#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>

namespace OS1 = ouster::OS1;
namespace viz = ouster::viz;

std::string getMilliTime();

int seq = 1;
int count = 0;
pcl::PointCloud<pcl::PointXYZ> cloud; //생성할 PointCloud structure구조체(x,y,z) 정의
pcl::PointCloud<pcl::PointXYZ> cloud2;
int W = 1024;
int H = OS1::pixels_per_column;
std::string metadata{};
std::string path = "/home/kanakim/Documents/LiDAR/i30_LiDAR_ts_" + getMilliTime() + ".txt";
std::ofstream writeFile(path.data());
Linux_keyboard lk;

/*timestamp*/
std::string getMilliTime(){
    auto time = std::chrono::system_clock::now();
    auto mill = std::chrono::duration_cast<std::chrono::milliseconds>
    (time.time_since_epoch());
    long long currentTimeMillis = mill.count();
    int msc = currentTimeMillis % 1000;
    long nowTime = currentTimeMillis/1000;
    tm *ts = localtime(&nowTime);
    char buffer[80];
    strftime(buffer, 80, "%Y%m%d%H%M%S", ts);
    std::string s(buffer);
    std::string str;
    if(stoi(std::to_string(msc)) < 10){
        str = s+"00"+std::to_string(msc);
    }
    else if(stoi(std::to_string(msc)) < 100){
        str = s+"0"+std::to_string(msc);
    }else{
        str = s+std::to_string(msc);
    }
    return str;
}

/*save pcd*/
void WritePCD(std::vector<double> xyz_lut, uint8_t* buf){

    for (int icol = 0; icol < OS1::columns_per_buffer; icol++) {
        const uint8_t* col_buf = OS1::nth_col(icol, buf);
        const uint16_t m_id = OS1::col_measurement_id(col_buf);
        const int idx = H * m_id;

        for (uint8_t ipx = 0; ipx < H; ipx++) {
            const uint8_t* px_buf = OS1::nth_px(ipx, col_buf);
            uint32_t r = OS1::px_range(px_buf);
            int ind = 3 * (idx + ipx);
            pcl::PointXYZ LiDAR;

            LiDAR.x = r * 0.001f * xyz_lut[ind + 0];
            LiDAR.y = r * 0.001f * xyz_lut[ind + 1];
            LiDAR.z = r * 0.001f * xyz_lut[ind + 2];

            if(count > W*H) break;

            cloud.push_back(LiDAR);
            count++;
        }
    }

}

/**
 * Print usage
 */
void print_help() {
    std::cout
        << "Usage: viz [options] [hostname] [udp_destination]\n"
        << "Options:\n"
        << "  -m <512x10 | 512x20 | 1024x10 | 1024x20 | 2048x10> : lidar mode, "
           "default 1024x10\n"
        << "  -l <port> : use specified port for lidar data\n"
        << "  -i <port> : use specified port for imu data \n"
        << "  -f <path> : use provided metadata file; do not configure via TCP"
        << std::endl;
}


std::string read_metadata(const std::string& meta_file) {
    std::stringstream buf{};
    std::ifstream ifs{};
    ifs.open(meta_file);
    buf << ifs.rdbuf();
    ifs.close();

    if (!ifs) {
        std::cerr << "Failed to read " << meta_file
                  << "; check that the path is valid" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return buf.str();
}

int main(int argc, char** argv) {
    int W = 1024;
    int H = OS1::pixels_per_column;
    OS1::lidar_mode mode = OS1::MODE_1024x10;
    bool do_config = true;  // send tcp commands to configure sensor
    std::string metadata{};
    int lidar_port = 0;
    int imu_port = 0;

    try {
        int c = 0;
        while ((c = getopt(argc, argv, "hm:l:i:f:")) != -1) {
            switch (c) {
                case 'h':
                    print_help();
                    return 1;
                    break;
                case 'm':
                    mode = OS1::lidar_mode_of_string(optarg);
                    if (mode) {
                        W = OS1::n_cols_of_lidar_mode(mode);
                    } else {
                        std::cout << "Lidar Mode must be 512x10, 512x20, "
                                     "1024x10, 1024x20, or 2048x10"
                                  << std::endl;
                        print_help();
                        std::exit(EXIT_FAILURE);
                    }
                    break;
                case 'l':
                    lidar_port = std::stoi(optarg);
                    break;
                case 'i':
                    imu_port = std::stoi(optarg);
                    break;
                case 'f':
                    do_config = false;
                    metadata = read_metadata(optarg);
                    break;
                case '?':
                    std::cout << "Invalid Argument Format" << std::endl;
                    print_help();
                    std::exit(EXIT_FAILURE);
                    break;
            }
        }
    } catch (const std::exception& ex) {
        std::cout << "Invalid Argument Format: " << ex.what() << std::endl;
        print_help();
        std::exit(EXIT_FAILURE);
    }

    if (do_config && argc != optind + 2) {
        std::cerr << "Expected 2 arguments after options" << std::endl;
        print_help();
        std::exit(EXIT_FAILURE);
    }

    std::shared_ptr<OS1::client> cli;
    if (do_config) {
        std::string os1_host = argv[optind];
        std::string os1_udp_dest = argv[optind + 1];
        std::cout << "Configuring sensor: " << os1_host
                  << " UDP Destination:" << os1_udp_dest << std::endl;
        cli =
            OS1::init_client(os1_host, os1_udp_dest, mode,
                             OS1::TIME_FROM_INTERNAL_OSC, lidar_port, imu_port);
    } else {
        if (lidar_port == 0) lidar_port = 7502;
        if (imu_port == 0) imu_port = 7503;
        std::cout << "Listening for sensor data on udp ports " << lidar_port
                  << " and " << imu_port << std::endl;
        cli = OS1::init_client(lidar_port, imu_port);
    }

    if (!cli) {
        std::cerr << "Failed to initialize client" << std::endl;
        print_help();
        std::exit(EXIT_FAILURE);
    }

    uint8_t lidar_buf[OS1::lidar_packet_bytes + 1];
    uint8_t imu_buf[OS1::imu_packet_bytes + 1];

    auto ls = std::unique_ptr<ouster::LidarScan>(new ouster::LidarScan(W, H));

    auto vh = viz::init_viz(W, H);

    if (do_config) metadata = OS1::get_metadata(*cli);

    auto info = OS1::parse_metadata(metadata);

    auto xyz_lut = OS1::make_xyz_lut(W, H, info.beam_azimuth_angles,
                                     info.beam_altitude_angles);

    // Use to signal termination
    std::atomic_bool end_program{false};

    // auto it = std::back_inserter(*ls);
    auto it = ls->begin();

    // callback that calls update with filled lidar scan
    auto batch_and_update = OS1::batch_to_iter<ouster::LidarScan::iterator>(
        xyz_lut, W, H, ouster::LidarScan::Point::Zero(),
        &ouster::LidarScan::make_val, [&](uint64_t) {
            // swap lidar scan and point it to new buffer
            viz::update(*vh, ls);
            it = ls->begin();
        }
    );


    //lk.init_keyboard();


    // Start poll thread
    std::thread poll([&] {
        while (!end_program) {
            // Poll the client for data and add to our lidar scan
            OS1::client_state st = OS1::poll_client(*cli);
            if (st & OS1::client_state::ERROR) {
                std::cerr << "Client returned error state" << std::endl;
                std::exit(EXIT_FAILURE);
            }
            if (st & OS1::client_state::LIDAR_DATA) {
                if (OS1::read_lidar_packet(*cli, lidar_buf)){
                    batch_and_update(lidar_buf, it);
                    WritePCD(xyz_lut, lidar_buf);
                }
            }
            if (st & OS1::client_state::IMU_DATA) {
                OS1::read_imu_packet(*cli, imu_buf);
            }

            if(count == W*H){

                std::string ch_ts = getMilliTime();
                std::string name = "/home/kanakim/Documents/LiDAR/PCD/i30_LiDAR_" + ch_ts + ".pcd";
                pcl::io::savePCDFileASCII (name, cloud);
                cloud = {};
                count = 0;

                if(writeFile.is_open()){
                    writeFile << ch_ts << "\n";
                }
            }


            /*
            if(lk._kbhit()){//when esc input, break;
				int ch = lk._getch();
				if(ch>=10) break;
			}*/
        }
    });
/*
    std::thread poll2([&] {
        while (!end_program) {
            
        }
    }); 
*/







    // Start render loop
    viz::run_viz(*vh);
    end_program = true;

    //lk.close_keyboard();

    // clean up
    poll.join();
    //poll2.join();
    writeFile.close();
    return EXIT_SUCCESS;
}
