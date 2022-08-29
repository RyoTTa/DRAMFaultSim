//
// Created by ryotta205 on 8/29/22.
//

#ifndef DRAMFAULTSIM_STAT_H
#define DRAMFAULTSIM_STAT_H
#include "configuration.h"
#include <iomanip>
#include <fstream>

namespace dramfaultsim{
    class Stat{
    public:
        Stat(Config &config);
        ~Stat();

        void PrintStat();

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

        double hard_fault_rate;
        std::string address_mapping;
        std::string generator_system;
        std::string memory_system;
        std::string fault_model;
        std::string data_pattern_str;

        uint64_t executed_request_num;
        uint64_t fault_request_num;
        uint64_t correct_request_num;

        uint64_t single_fault_in_column_num;
        uint64_t double_fault_in_column_num;
        uint64_t triple_fault_in_column_num;
        uint64_t others_fault_in_column_num;
        uint64_t logical_adjacent_fault_in_column_num;

    private:
        Config &config_;
        std::string dir_path_;
        std::ofstream writer_;
    };
}
#endif //DRAMFAULTSIM_STAT_H