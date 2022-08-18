#ifndef DRAMFAULTSIM_CONFIG_H
#define DRAMFAULTSIM_CONFIG_H

#include <fstream>
#include <string>
#include "common.h"
#include "INIReader.h"

namespace dramfaultsim {

    class Config {

    public:
        //Basic Function
        Config(std::string config_file, std::string out_dir, uint64_t request);

        Address AddressMapping(uint64_t hex_addr) const;

        void PrintInfo();
        //DRAM physical address structure

        int channel_size;
        int channels;
        int ranks;
        int banks;
        int bankgroups;
        int banks_per_group;
        int rows;
        int columns;
        int device_width;
        int bus_width;
        int devices_per_rank;
        int BL;

        // Address mapping numbers
        int shift_bits;
        int ch_pos, ra_pos, bg_pos, ba_pos, ro_pos, co_pos;
        uint64_t ch_mask, ra_mask, bg_mask, ba_mask, ro_mask, co_mask;

        // System
        std::string address_mapping;
        std::string output_dir;
        std::string output_prefix;
        std::string txt_stats_name;

        // Computed parameters
        int request_size_bytes;

        // Running Request Number
        uint64_t num_request;

    private:
        INIReader *reader_;

        void CalculateSize();

        int GetInteger(const std::string &sec, const std::string &opt,
                       int default_val) const;

        void InitDRAMParams();

        void InitSystemParams();

        void SetAddressMapping();


    };


}
#endif //DRAMFAULTSIM_CONFIG_H