//
// Created by ryotta205 on 8/24/22.
//
#include "faultmap.h"

namespace dramfaultsim {
    FaultMapReader::FaultMapReader(Config &config, std::string path, FaultStruct ******fault_map)
            : config_(config), path_(path), fault_map_(fault_map) {

        reader_.open(path_, std::ios::in | std::ios::binary);
        if (!reader_) {
            std::cerr << "Can't read fault map file - " << path_ << std::endl;
            AbruptExit(__FILE__, __LINE__);
        }
    }

    FaultStruct ******FaultMapReader::Read() {
        reader_.read((char *) &channel_size, sizeof(config_.channel_size));
        reader_.read((char *) &channels, sizeof(config_.channels));
        reader_.read((char *) &ranks, sizeof(config_.ranks));
        reader_.read((char *) &bankgroups, sizeof(config_.bankgroups));
        reader_.read((char *) &banks_per_group, sizeof(config_.banks_per_group));
        reader_.read((char *) &rows, sizeof(config_.rows));
        reader_.read((char *) &columns, sizeof(config_.columns));
        reader_.read((char *) &device_width, sizeof(config_.device_width));
        reader_.read((char *) &bus_width, sizeof(config_.bus_width));
        reader_.read((char *) &devices_per_rank, sizeof(config_.devices_per_rank));
        reader_.read((char *) &BL, sizeof(config_.BL));

#ifdef TEST_MODE
        std::cout << "####TEST_MODE OUTPUT" << std::endl;
        std::cout << "####Read Fault Map Information" << std::endl;
        std::cout << "Channels: " << channels << std::endl;
        std::cout << "Ranks: " << ranks << std::endl;
        std::cout << "BankGroups: " << bankgroups << std::endl;
        std::cout << "Bank Per Group: " << banks_per_group << std::endl;
        std::cout << "Rows: " << rows << std::endl;
        std::cout << "Columns: " << columns << std::endl;
        std::cout << "Device Width: " << device_width << std::endl;
        std::cout << "Bus Width: " << bus_width << std::endl;
        std::cout << "Devices Per Rank: " << devices_per_rank << std::endl;
        std::cout << "Devices Per BL: " << BL << std::endl;
#endif
        if (config_.channel_size != channel_size ||
            config_.channels != channels ||
            config_.ranks != ranks ||
            config_.bankgroups != bankgroups ||
            config_.banks_per_group != banks_per_group ||
            config_.rows != rows ||
            config_.columns != columns ||
            config_.device_width != device_width ||
            config_.bus_width != bus_width ||
            config_.devices_per_rank != devices_per_rank ||
            config_.BL != BL) {

            std::cerr << "Memory Configuration does not match (FaultMap - Config)" << path_ << std::endl;
            AbruptExit(__FILE__, __LINE__);
        }

        for (int i = 0; i < config_.channels; i++) {
            for (int j = 0; j < config_.ranks; j++) {
                for (int k = 0; k < config_.bankgroups; k++) {
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        for (int e = 0; e < config_.rows; e++) {
                            for (int f = 0; f < config_.columns; f++) {
                                reader_.read((char *) &(fault_map_[i][j][k][q][e][f].hardfault),
                                             sizeof(fault_map_[i][j][k][q][e][f].hardfault));
                                reader_.read((char *) &(fault_map_[i][j][k][q][e][f].vrt_low),
                                             sizeof(fault_map_[i][j][k][q][e][f].vrt_low));
                                reader_.read((char *) &(fault_map_[i][j][k][q][e][f].vrt_mid),
                                             sizeof(fault_map_[i][j][k][q][e][f].vrt_mid));
                                reader_.read((char *) &(fault_map_[i][j][k][q][e][f].vrt_high),
                                             sizeof(fault_map_[i][j][k][q][e][f].vrt_high));
                            }
                        }
                    }
                }
            }
        }

        return fault_map_;
    }


    FaultMapWriter::FaultMapWriter(Config &config, std::string path, FaultStruct ******fault_map)
            : config_(config), path_(path), fault_map_(fault_map) {

        writer_.open(path_, std::ios::out | std::ios::trunc | std::ios::binary);
        if (writer_.fail()) {
            std::cerr << "Can't write fault map file - " << path_ << std::endl;
            AbruptExit(__FILE__, __LINE__);
        }
    }

    bool FaultMapWriter::Write() {
        bool rtn_value = false;

        writer_.write((char *) &config_.channel_size, sizeof(config_.channel_size));
        writer_.write((char *) &config_.channels, sizeof(config_.channels));
        writer_.write((char *) &config_.ranks, sizeof(config_.ranks));
        writer_.write((char *) &config_.bankgroups, sizeof(config_.bankgroups));
        writer_.write((char *) &config_.banks_per_group, sizeof(config_.banks_per_group));
        writer_.write((char *) &config_.rows, sizeof(config_.rows));
        writer_.write((char *) &config_.columns, sizeof(config_.columns));
        writer_.write((char *) &config_.device_width, sizeof(config_.device_width));
        writer_.write((char *) &config_.bus_width, sizeof(config_.bus_width));
        writer_.write((char *) &config_.devices_per_rank, sizeof(config_.devices_per_rank));
        writer_.write((char *) &config_.BL, sizeof(config_.BL));

        for (int i = 0; i < config_.channels; i++) {
            for (int j = 0; j < config_.ranks; j++) {
                for (int k = 0; k < config_.bankgroups; k++) {
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        for (int e = 0; e < config_.rows; e++) {
                            for (int f = 0; f < config_.columns; f++) {
                                //fault_map_[i][j][k][q][e][f].hardfault = 0x0;
                                //fault_map_[i][j][k][q][e][f].vrt_low = 0x0;
                                //fault_map_[i][j][k][q][e][f].vrt_mid = 0x0;
                                //fault_map_[i][j][k][q][e][f].vrt_high = 0x0;
                                writer_.write((char *) &(fault_map_[i][j][k][q][e][f].hardfault),
                                             sizeof(fault_map_[i][j][k][q][e][f].hardfault));
                                writer_.write((char *) &(fault_map_[i][j][k][q][e][f].vrt_low),
                                             sizeof(fault_map_[i][j][k][q][e][f].vrt_low));
                                writer_.write((char *) &(fault_map_[i][j][k][q][e][f].vrt_mid),
                                             sizeof(fault_map_[i][j][k][q][e][f].vrt_mid));
                                writer_.write((char *) &(fault_map_[i][j][k][q][e][f].vrt_high),
                                             sizeof(fault_map_[i][j][k][q][e][f].vrt_high));

                            }
                        }
                    }
                }
            }
        }

        return rtn_value;
    }

}