//
// Created by ryotta205 on 8/19/22.
//
#include "faultmodel.h"
#include <thread>

namespace dramfaultsim {
    NaiveFaultModel::NaiveFaultModel(Config &config, uint64_t ******data_block)
            : FaultModel(config, data_block) {
#ifndef TEST_MODE
        std::mt19937_64 gen(rd());
#endif
        //data_block_[Channel][Rank][BankGourp][Bank][Row][Col]
        fault_map_ = new FaultStruct *****[config_.channels];

        for (int i = 0; i < config_.channels; i++) {
            fault_map_[i] = new FaultStruct ****[config_.ranks];
            for (int j = 0; j < config_.ranks; j++) {
                fault_map_[i][j] = new FaultStruct ***[config_.bankgroups];
                for (int k = 0; k < config_.bankgroups; k++) {
                    fault_map_[i][j][k] = new FaultStruct **[config_.banks_per_group];
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        fault_map_[i][j][k][q] = new FaultStruct *[config_.rows];
                        for (int e = 0; e < config_.rows; e++) {
                            fault_map_[i][j][k][q][e] = new FaultStruct[config_.columns];
                            for (int f = 0; f < config_.columns; f++) {
                                //fault_map_[i][j][k][q][e][f].hardfault = 0xffffffffffffffff;
                                fault_map_[i][j][k][q][e][f].hardfault = 0x0;
                                fault_map_[i][j][k][q][e][f].vrt_low = 0x0;
                                fault_map_[i][j][k][q][e][f].vrt_mid = 0x0;
                                fault_map_[i][j][k][q][e][f].vrt_high = 0x0;
                            }
                        }
                    }
                }
            }
        }

        ErrorMask = 0;
        num_all_cell = 0;
        num_hard_fault_cell = 0;

#ifdef TEST_MODE
        std::cout << "HardFaultGenerator" << std::endl;
#endif
        HardFaultGenerator();
#ifdef TEST_MODE
        std::cout << "VRTFaultGenerator" << std::endl;
#endif
        VRTErrorGenerator();
    }

    NaiveFaultModel::~NaiveFaultModel() {
        if (!config_.faultmap_write_path.empty()) {
            FaultMapWriter *writer_ = new FaultMapWriter(config_, config_.faultmap_write_path,
            fault_map_);

            std::cout << "Write On" << std::endl;

            writer_->Write();
        }

        std::cout << "NaiveFaultModel destructor" << std::endl;

        for (int i = 0; i < config_.channels; i++) {
            for (int j = 0; j < config_.ranks; j++) {
                for (int k = 0; k < config_.bankgroups; k++) {
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        for (int e = 0; e < config_.rows; e++) {
                            delete[] fault_map_[i][j][k][q][e];
                        }
                        delete[] fault_map_[i][j][k][q];
                    }
                    delete[] fault_map_[i][j][k];
                }
                delete[] fault_map_[i][j];
            }
            delete[] fault_map_[i];
        }
        delete[] fault_map_;
    }

    uint64_t NaiveFaultModel::ErrorInjection(uint64_t addr) {

        ErrorMask = (uint64_t) 0;
        SetRecvAddress(addr);


        ErrorMask |= HardFaultError();

        return ErrorMask;
    }

    uint64_t NaiveFaultModel::HardFaultError() {
        return fault_map_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup] \
[recv_addr_bank][recv_addr_row][recv_addr_column].hardfault;
    }

    void NaiveFaultModel::HardFaultGenerator() {
        num_all_cell =
                (uint64_t) config_.channels * (uint64_t) config_.ranks * (uint64_t) config_.bankgroups *
                (uint64_t) config_.banks_per_group * (uint64_t) config_.rows *
                (uint64_t) config_.columns * (uint64_t) config_.bus_width;

        num_hard_fault_cell = (uint64_t) ((double) num_all_cell * config_.hard_fault_rate / 100);

        if (config_.thread_model == "SingleThread") {
            HardFaultGeneratorThread(num_hard_fault_cell);
        } else if (config_.thread_model == "MultiThread") {
            std::thread _t[config_.thread_num];

            for (int i = 0; i < config_.thread_num; i++) {
                _t[i] = std::thread(&NaiveFaultModel::HardFaultGeneratorThread, this,
                                    num_hard_fault_cell / (uint64_t) config_.thread_num);
            }
            for (int i = 0; i < config_.thread_num; i++) {
                _t[i].join();
            }


        }

    }

    void NaiveFaultModel::HardFaultGeneratorThread(uint64_t num_generate) {
        for (uint64_t i = 0; i < num_generate; i++) {
            int channel, rank, bankgroup, bankpergroup, row, column, bit;
            channel = GetRandomInt(0, (config_.channels - 1));
            rank = GetRandomInt(0, (config_.ranks - 1));
            bankgroup = GetRandomInt(0, (config_.bankgroups - 1));
            bankpergroup = GetRandomInt(0, (config_.banks_per_group - 1));
            row = GetRandomInt(0, (config_.rows - 1));
            column = GetRandomInt(0, (config_.columns - 1));
            bit = GetRandomInt(0, (config_.bus_width - 1));

            fault_map_[channel][rank][bankgroup][bankpergroup][row][column].hardfault |= ((uint64_t) 1 << bit);
            //std::cout << channel << rank << bankgroup << bankpergroup << row << column << bit << std::endl;
            //std::cout<<fault_map_[channel][rank][bankgroup][bankpergroup][row][column].hardfault<<std::endl;
            //std::cout << i << std::endl;
        }
    }

    void NaiveFaultModel::VRTErrorGenerator() {
        num_vrt_fault_cell = num_hard_fault_cell;

        num_vrt_fault_low_cell = num_vrt_fault_cell / 3;
        num_vrt_fault_mid_cell = num_vrt_fault_cell / 3;
        num_vrt_fault_high_cell = num_vrt_fault_cell / 3;

        //std::cout << num_all_cell << std::endl;
        //std::cout << num_hard_fault_cell << std::endl;

        if (config_.thread_model == "SingleThread") {
            VRTErrorGeneratorThread(num_vrt_fault_low_cell, num_vrt_fault_mid_cell,
                                    num_vrt_fault_high_cell);
        } else if (config_.thread_model == "MultiThread") {
            std::thread _t[config_.thread_num];

            for (int i = 0; i < config_.thread_num; i++) {
                _t[i] = std::thread(&NaiveFaultModel::VRTErrorGeneratorThread, this,
                                    num_vrt_fault_low_cell / (uint64_t) config_.thread_num,
                                    num_vrt_fault_mid_cell / (uint64_t) config_.thread_num,
                                    num_vrt_fault_high_cell / (uint64_t) config_.thread_num);
            }
            for (int i = 0; i < config_.thread_num; i++) {
                _t[i].join();
            }
        }

    }

    void NaiveFaultModel::VRTErrorGeneratorThread(uint64_t num_generate_low, uint64_t num_generate_mid,
                                                  uint64_t num_generate_high) {
        for (uint64_t i = 0; i < num_generate_low; i++) {
            int channel, rank, bankgroup, bankpergroup, row, column, bit;
            channel = GetRandomInt(0, (config_.channels - 1));
            rank = GetRandomInt(0, (config_.ranks - 1));
            bankgroup = GetRandomInt(0, (config_.bankgroups - 1));
            bankpergroup = GetRandomInt(0, (config_.banks_per_group - 1));
            row = GetRandomInt(0, (config_.rows - 1));
            column = GetRandomInt(0, (config_.columns - 1));
            bit = GetRandomInt(0, (config_.bus_width - 1));

            fault_map_[channel][rank][bankgroup][bankpergroup][row][column].vrt_low |= ((uint64_t) 1 << bit);
        }

        for (uint64_t i = 0; i < num_generate_mid; i++) {
            int channel, rank, bankgroup, bankpergroup, row, column, bit;
            channel = GetRandomInt(0, (config_.channels - 1));
            rank = GetRandomInt(0, (config_.ranks - 1));
            bankgroup = GetRandomInt(0, (config_.bankgroups - 1));
            bankpergroup = GetRandomInt(0, (config_.banks_per_group - 1));
            row = GetRandomInt(0, (config_.rows - 1));
            column = GetRandomInt(0, (config_.columns - 1));
            bit = GetRandomInt(0, (config_.bus_width - 1));

            fault_map_[channel][rank][bankgroup][bankpergroup][row][column].vrt_mid |= ((uint64_t) 1 << bit);
        }

        for (uint64_t i = 0; i < num_generate_high; i++) {
            int channel, rank, bankgroup, bankpergroup, row, column, bit;
            channel = GetRandomInt(0, (config_.channels - 1));
            rank = GetRandomInt(0, (config_.ranks - 1));
            bankgroup = GetRandomInt(0, (config_.bankgroups - 1));
            bankpergroup = GetRandomInt(0, (config_.banks_per_group - 1));
            row = GetRandomInt(0, (config_.rows - 1));
            column = GetRandomInt(0, (config_.columns - 1));
            bit = GetRandomInt(0, (config_.bus_width - 1));

            fault_map_[channel][rank][bankgroup][bankpergroup][row][column].vrt_high |= ((uint64_t) 1 << bit);
        }
    }
}