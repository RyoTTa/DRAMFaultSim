#include <fstream>
#include <string>
#include "common.h"

namespace dramfaultsim{

class Config{
    public:
    Config(std::string config_file, std::string out_dir);
    Address AddressMapping(uint64_t hex_addr) const;
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
    
};


}