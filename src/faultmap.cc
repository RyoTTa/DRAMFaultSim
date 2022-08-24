//
// Created by ryotta205 on 8/24/22.
//
#include "faultmap.h"

namespace dramfaultsim{
    FaultMapReader::FaultMapReader(Config &config, std::string path, FaultStruct ****** fault_map)
        : config_(config), path_(path), fault_map_(fault_map) {

        reader_.open(path_, std::ios::in | std::ios::binary);
        if (!reader_){
            std::cerr << "Can't read fault map file - " << path_ << std::endl;
            AbruptExit(__FILE__, __LINE__);
        }
    }

    FaultMapWriter::FaultMapWriter(Config &config, std::string path, FaultStruct ****** fault_map)
            : config_(config), path_(path), fault_map_(fault_map) {

        writer_.open(path_, std::ios::out | std::ios::trunc | std::ios::binary);
        if (writer_.fail()){
            std::cerr << "Can't write fault map file - " << path_ << std::endl;
            AbruptExit(__FILE__, __LINE__);
        }

    }

    bool FaultMapWriter::Write() {
        bool rtn_value = false;

        writer_.write((char*)&config_.channel_size, sizeof(config_.channel_size));
        writer_.write((char*)&config_.channels, sizeof(config_.channels));
        writer_.write((char*)&config_.channel_size, sizeof(config_.channel_size));
        writer_.write((char*)&config_.bankgroups, sizeof(config_.bankgroups));
        writer_.write((char*)&config_.banks_per_group, sizeof(config_.banks_per_group));
        writer_.write((char*)&config_.rows, sizeof(config_.rows));
        writer_.write((char*)&config_.columns, sizeof(config_.columns));
        writer_.write((char*)&config_.devices_per_rank, sizeof(config_.devices_per_rank));

        return rtn_value;
    }
}