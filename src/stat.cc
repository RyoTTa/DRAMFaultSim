//
// Created by ryotta205 on 8/29/22.
//
#include "stat.h"


namespace dramfaultsim {
    Stat::Stat(Config &config)
            : config_(config) {
        channel_size = config_.channel_size;
        channels = config_.channels;
        ranks = config_.ranks;
        banks = config_.banks;
        bankgroups = config_.bankgroups;
        banks_per_group = config_.banks_per_group;
        rows = config_.rows;
        columns = config_.columns;
        device_width = config_.device_width;
        bus_width = config_.bus_width;
        devices_per_rank = config_.devices_per_rank;
        BL = config_.BL;

        fault_rate = config_.fault_rate;
        address_mapping = config_.address_mapping;
        generator_system = config_.generator_system;
        memory_system = config_.memory_system;
        fault_model = config_.fault_model;
        data_pattern_str = config_.data_pattern_str;

    }

    void Stat::PrintStat() {
        writer_.open(config_.output_dir + "/" + config_.output_prefix
                     + "_Round_" + std::to_string(repeat_round) + "_Stats.txt",
                     std::ios::out | std::ios::trunc);
        if (writer_.fail()) {
            std::cerr << "Can't write stats file - " << config_.output_dir << "/" << config_.output_prefix
                      << "_Round_" << repeat_round << "_Stats.txt" << std::endl;
            AbruptExit(__FILE__, __LINE__);
        }

        std::cout.setf(std::ios::left);
        writer_ << std::setw(40) << "Channel Size: " << channel_size << "\n"
                << std::setw(40) << "Channels: " << channels << "\n"
                << std::setw(40) << "Ranks: " << ranks << "\n"
                << std::setw(40) << "Banks Groups: " << bankgroups << "\n"
                << std::setw(40) << "Banks Per Group: " << banks_per_group << "\n"
                << std::setw(40) << "Rows: " << rows << "\n"
                << std::setw(40) << "Columns: " << columns << "\n"
                << std::setw(40) << "Device Width: " << device_width << "\n"
                << std::setw(40) << "Bus Width: " << bus_width << "\n"
                << std::setw(40) << "Devices Per Rank: " << devices_per_rank << "\n"
                << std::setw(40) << "BL: " << BL << "\n"
                << std::setw(40) << "Address Mapping: " << address_mapping << "\n"
                << std::setw(40) << "Generator System: " << generator_system << "\n"
                << std::setw(40) << "Memory System: " << memory_system << "\n"
                << std::setw(40) << "Fault Model: " << fault_model << "\n"
                << std::setw(40) << "Hard Fault Rate: " << fault_rate << "\n"
                << std::setw(40) << "Data Pattern: " << data_pattern_str << "\n"

                << std::setw(40) << "Number of Executed Requests: " << executed_request_num << "\n"
                << std::setw(40) << "Number of Fault Requests: " << fault_request_num << "\n"
                << std::setw(40) << "Number of Correct Request: " << correct_request_num << "\n"
                << std::setw(40) << "Fault Rate of Request(BL): "
                << (double) fault_request_num / (double) executed_request_num << "\n"
                << std::setw(40) << "Number of Executed Columns: " << executed_column_num << "\n"
                << std::setw(40) << "Number of Fault Columns: " << fault_column_num << "\n"
                << std::setw(40) << "Number of Correct Columns: " << correct_column_num << "\n"
                << std::setw(40) << "Fault Rate of Column: " << (double) fault_column_num / (double) executed_column_num
                << "\n"

                << std::setw(40) << "Number of Fault Bits: " << all_fault_bit_num << "\n"
                << std::setw(40) << "Number of HardFault Bits: " << hard_fault_bit_num << "\n"
                << std::setw(40) << "Number of VRTFault Bits: " << vrt_fault_bit_num << "\n"
                << std::endl;


        writer_.close();
    }

    void Stat::ResetStat() {
        executed_request_num = 0;
        fault_request_num = 0;
        correct_request_num = 0;

        executed_column_num = 0;
        fault_column_num = 0;
        correct_column_num = 0;

        all_fault_bit_num = 0;
        hard_fault_bit_num = 0;
        vrt_fault_bit_num = 0;

        all_fault_bit_num = 0;

        single_fault_in_column_num = 0;
        double_fault_in_column_num = 0;
        triple_fault_in_column_num = 0;
        others_fault_in_column_num = 0;

        logical_adjacent_fault_in_column_num = 0;

    }
}


