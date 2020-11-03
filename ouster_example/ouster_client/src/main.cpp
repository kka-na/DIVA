#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <pcap/pcap.h>
#include <cmath>
#include <chrono>

#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>

#include "ouster/os1.h"
#include "ouster/os1_packet.h"
#include "ouster/os1_util.h"

namespace OS1 = ouster::OS1;

/*data packets*/
uint64_t n_lidar_packets = 0;
uint64_t n_imu_packets = 0;

pcl::PointCloud<pcl::PointXYZ> cloud; //생성할 PointCloud structure구조체(x,y,z) 정의 

int count = 0;
int seq = 1;


 /*std::thread poll2([&] {
        while (!end_program) {
            OS1::client_state st = OS1::poll_client(*cli);
            if (OS1::read_lidar_packet(*cli, lidar_buf)){
                    int next_m_id{W};
                    int32_t cur_f_id{-1};

                    int64_t scan_ts{-1L};
                    for (int icol = 0; icol < OS1::columns_per_buffer; icol++) {
                        const uint8_t* col_buf = OS1::nth_col(icol, lidar_buf);
                        const uint16_t m_id = OS1::col_measurement_id(col_buf);
                        const uint16_t f_id = OS1::col_frame_id(col_buf);
                        const uint64_t ts = OS1::col_timestamp(col_buf);
                        const bool valid = OS1::col_valid(col_buf) == 0xffffffff;

                        if (!valid || m_id >= W || f_id + 1 == cur_f_id) continue;
                        if (f_id != cur_f_id) {

                        // start new frame
                            scan_ts = ts;
                            next_m_id = 0;
                            cur_f_id = f_id;
                        }
                        if (m_id >= next_m_id) {
                            std::fill(it + (H * next_m_id), it + (H * m_id), ouster::LidarScan::Point::Zero());
                            next_m_id = m_id + 1;
                        }
                        const int idx = H * m_id;
                        for (uint8_t ipx = 0; ipx < H; ipx++) {
                        const uint8_t* px_buf = OS1::nth_px(ipx, col_buf);
                        uint32_t r = OS1::px_range(px_buf);
                        int ind = 3 * (idx + ipx);

                    // x, y, z(m), i, ts, reflectivity, ring, noise, range (mm)
                        std::cout << "\r" <<
                                "x:     " << r * 0.001f * xyz_lut[ind + 0] << std::setw(20) <<
                                "y:     " << r * 0.001f * xyz_lut[ind + 1] << std::setw(20) <<
                                "z:     " << r * 0.001f * xyz_lut[ind + 2] << std::setw(20) <<
                                "sign: " << OS1::px_signal_photons(px_buf) << std::setw(20) <<
                                "time: " << ts - scan_ts << std::setw(20) <<
                                "refl: " << OS1::px_reflectivity(px_buf) << std::setw(20) <<
                                "nois: " << OS1::px_noise_photons(px_buf) << std::endl;
                                std::flush(std::cout);
                        } 
                    }
                }
        }
    });*/

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
void WritePCD(std::shared_ptr<OS1::client> cli, uint8_t* buf){
    int W = 1024;
    int H = OS1::pixels_per_column;
    std::string metadata{};
    metadata = OS1::get_metadata(*cli);
    auto info = OS1::parse_metadata(metadata);
    auto xyz_lut = OS1::make_xyz_lut(W, H, info.beam_azimuth_angles,
                                     info.beam_altitude_angles);

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

            cloud.push_back(LiDAR);
            count++;
        }
    }

    if(count == W*H){
        std::string name = "pcd/" + std::to_string(seq) + ".pcd";
        pcl::io::savePCDFileASCII (name, cloud);
        seq++;
        cloud = {};
        count = 0;

        std::string ch_ts = getMilliTime();
        std::string path = "timestamp/" + ch_ts + ".txt";
        std::ofstream writeFile(path.data());
        if(writeFile.is_open()){
            writeFile << getMilliTime() << "\n";
            writeFile.close();
        }
        std::cout << ch_ts << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: ouster_client_example <os1_hostname> "
                     "<data_destination_ip>"
                  << std::endl;
        return 1;
    }

    auto cli = OS1::init_client(argv[1], argv[2]);
    if (!cli) {
        std::cerr << "Failed to connect to client at: " << argv[1] << std::endl;
        return 1;
    }

    uint8_t lidar_buf[OS1::lidar_packet_bytes + 1];
    uint8_t imu_buf[OS1::imu_packet_bytes + 1];

    while (true) {
        OS1::client_state st = OS1::poll_client(*cli);

        if (st & OS1::ERROR) {
            return 1;
        } else if (st & OS1::LIDAR_DATA) {
            if (OS1::read_lidar_packet(*cli, lidar_buf)){
                n_lidar_packets++;
                WritePCD(cli, lidar_buf);
            }
               

        } else if (st & OS1::IMU_DATA) {
            if (OS1::read_imu_packet(*cli, imu_buf)) n_imu_packets++;
        } 
    }
    return 0;
}
