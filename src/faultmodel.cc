//
// Created by ryotta205 on 8/19/22.
//
#include "faultmodel.h"
#include <thread>
#include <bitset>

namespace dramfaultsim {
    NaiveFaultModel::NaiveFaultModel(Config &config, uint64_t *******data_block, Stat &stat)
            : FaultModel(config, data_block, stat) {
#ifndef TEST_MODE
        std::mt19937_64 gen(rd());
#endif

#ifdef TEST_MODE
        std::cout << "NaiveFaultModel" << std::endl;
#endif
        //data_block_[Channel][Rank][BankGourp][Bank][Row][Act_Col][BL]
        fault_map_ = new FaultStruct ******[config_.channels];

        for (int i = 0; i < config_.channels; i++) {
            fault_map_[i] = new FaultStruct *****[config_.ranks];
            for (int j = 0; j < config_.ranks; j++) {
                fault_map_[i][j] = new FaultStruct ****[config_.bankgroups];
                for (int k = 0; k < config_.bankgroups; k++) {
                    fault_map_[i][j][k] = new FaultStruct ***[config_.banks_per_group];
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        fault_map_[i][j][k][q] = new FaultStruct **[config_.rows];
                        for (int e = 0; e < config_.rows; e++) {
                            fault_map_[i][j][k][q][e] = new FaultStruct *[config_.actual_colums];
                            for (int w = 0; w < config_.actual_colums; w++) {
                                fault_map_[i][j][k][q][e][w] = new FaultStruct[config_.BL];
                                for (int f = 0; f < config_.BL; f++) {
                                    fault_map_[i][j][k][q][e][w][f] = {0, 0};
                                }
                            }
                        }
                    }
                }
            }
        }

        ErrorMask = new uint64_t[config_.BL];

        if (!config_.faultmap_read_path.empty()) {
            FaultMapReader *reader_ = new FaultMapReader(config_, config_.faultmap_read_path,
                                                         fault_map_);
#ifdef TEST_MODE
            std::cout << "FaultMap Reader" << std::endl;
#endif
            fault_map_ = reader_->Read();

            delete reader_;
        } else {
#ifdef TEST_MODE
            std::cout << "HardFaultGenerator" << std::endl;
#endif
            HardFaultGenerator();
#ifdef TEST_MODE
            std::cout << "VRTFaultGenerator" << std::endl;
#endif
            VRTErrorGenerator();
        }

        num_all_cell = 0;
        num_hard_fault_cell = 0;
    }

    NaiveFaultModel::~NaiveFaultModel() {
#ifdef TEST_MODE
        std::cout << "NaiveFaultModel destructor" << std::endl;
#endif

        if (!config_.faultmap_write_path.empty()) {
            FaultMapWriter *writer_ = new FaultMapWriter(config_, config_.faultmap_write_path,
                                                         fault_map_);

#ifdef TEST_MODE
            std::cout << "FaultMap Writer" << std::endl;
#endif
            writer_->Write();

            delete writer_;
        }


        for (int i = 0; i < config_.channels; i++) {
            for (int j = 0; j < config_.ranks; j++) {
                for (int k = 0; k < config_.bankgroups; k++) {
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        for (int e = 0; e < config_.rows; e++) {
                            for (int w = 0; w < config_.actual_colums; w++) {
                                delete[] fault_map_[i][j][k][q][e][w];
                            }
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

    uint64_t *NaiveFaultModel::ErrorInjection(uint64_t addr) {

        for (int i = 0; i < config_.BL; i++) {
            ErrorMask[i] = 0;
        }
        SetRecvAddress(addr);

        HardFaultError();

        VRTFaultError();

        return ErrorMask;
    }

    void NaiveFaultModel::HardFaultError() {
        for (int i = 0; i < config_.BL; i++) {
            ErrorMask[i] |= fault_map_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i].hardfault;

            for (int j = 0; j < config_.bus_width; j++) {
                if (((ErrorMask[i]) & ((uint64_t) 1) << j)) {
                    stat_.hard_fault_bit_num++;
                }
            }
        }
    }

    void NaiveFaultModel::VRTFaultError() {
        int rate = GetRandomInt(1, 1000);

        for (int i = 0; i < config_.BL; i++) {
            if (fault_map_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i].vrt_size ==
                0)
                continue;
            for (int j = 0; j <
                            fault_map_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i].vrt_size; j++) {
                if (fault_map_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i].vrt[j].second >=
                    rate) {
                    //std::cout << std::bitset<64>(ErrorMask[i]) << std::endl;
                    //std::cout << (int)fault_map_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i].vrt[j].first<< std::endl;
                    //std::cout << "Rate : " << rate << "Threshold : " << fault_map_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i].vrt[j].second << std::endl;
                    ErrorMask[i] |= (uint64_t) 1 << ((config_.bus_width - 1) -
                                                     fault_map_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i].vrt[j].first);
                    stat_.vrt_fault_bit_num++;
                    //std::cout << std::bitset<64>(ErrorMask[i]) << std::endl << std::endl;
                }
            }
        }
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
            int channel, rank, bankgroup, bankpergroup, row, column, bl, bit;
            channel = GetRandomInt(0, (config_.channels - 1));
            rank = GetRandomInt(0, (config_.ranks - 1));
            bankgroup = GetRandomInt(0, (config_.bankgroups - 1));
            bankpergroup = GetRandomInt(0, (config_.banks_per_group - 1));
            row = GetRandomInt(0, (config_.rows - 1));
            column = GetRandomInt(0, (config_.actual_colums - 1));
            bl = GetRandomInt(0, config_.BL - 1);
            bit = GetRandomInt(0, (config_.bus_width - 1));

            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].hardfault |= ((uint64_t) 1 << bit);
            //std::cout << channel << rank << bankgroup << bankpergroup << row << column << bit << std::endl;
            //std::cout<<fault_map_[channel][rank][bankgroup][bankpergroup][row][column].hardfault<<std::endl;
            //std::cout << i << std::endl;
        }
    }

    void NaiveFaultModel::VRTErrorGenerator() {
        num_vrt_fault_cell = num_hard_fault_cell;

        num_vrt_fault_low_low_cell = num_vrt_fault_cell / 5;
        num_vrt_fault_low_cell = num_vrt_fault_cell / 5;
        num_vrt_fault_mid_cell = num_vrt_fault_cell / 5;
        num_vrt_fault_high_cell = num_vrt_fault_cell / 5;
        num_vrt_fault_high_high_cell = num_vrt_fault_cell / 5;

        //std::cout << num_all_cell << std::endl;
        //std::cout << num_hard_fault_cell << std::endl;

        if (config_.thread_model == "SingleThread") {
            VRTErrorGeneratorThread(num_vrt_fault_low_low_cell, num_vrt_fault_low_cell,
                                    num_vrt_fault_mid_cell, num_vrt_fault_high_cell, num_vrt_fault_high_high_cell);
        } else if (config_.thread_model == "MultiThread") {
            std::thread _t[config_.thread_num];

            for (int i = 0; i < config_.thread_num; i++) {
                _t[i] = std::thread(&NaiveFaultModel::VRTErrorGeneratorThread, this,
                                    num_vrt_fault_low_low_cell / (uint64_t) config_.thread_num,
                                    num_vrt_fault_low_cell / (uint64_t) config_.thread_num,
                                    num_vrt_fault_mid_cell / (uint64_t) config_.thread_num,
                                    num_vrt_fault_high_cell / (uint64_t) config_.thread_num,
                                    num_vrt_fault_high_high_cell / (uint64_t) config_.thread_num);
            }
            for (int i = 0; i < config_.thread_num; i++) {
                _t[i].join();
            }
        }

    }

    void NaiveFaultModel::VRTErrorGeneratorThread(uint64_t num_generate_low_low, uint64_t num_generate_low,
                                                  uint64_t num_generate_mid, uint64_t num_generate_high,
                                                  uint64_t num_generate_high_high) {

        uint16_t rate;

        for (uint64_t i = 0; i < num_generate_low_low; i++) {
            int channel, rank, bankgroup, bankpergroup, row, column, bl, bit;
            channel = GetRandomInt(0, (config_.channels - 1));
            rank = GetRandomInt(0, (config_.ranks - 1));
            bankgroup = GetRandomInt(0, (config_.bankgroups - 1));
            bankpergroup = GetRandomInt(0, (config_.banks_per_group - 1));
            row = GetRandomInt(0, (config_.rows - 1));
            column = GetRandomInt(0, (config_.actual_colums - 1));
            bl = GetRandomInt(0, config_.BL - 1);
            bit = GetRandomInt(0, (config_.bus_width - 1));

            rate = (uint16_t) (std::abs(GetNormalInt(0, 1)) + 1);
            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].vrt_size++;
            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].vrt.push_back(
                    std::make_pair(bit, rate));

            //std::cout <<"Low : " << bit << "   " << rate << "\n";
        }

        for (uint64_t i = 0; i < num_generate_low; i++) {
            int channel, rank, bankgroup, bankpergroup, row, column, bl, bit;
            channel = GetRandomInt(0, (config_.channels - 1));
            rank = GetRandomInt(0, (config_.ranks - 1));
            bankgroup = GetRandomInt(0, (config_.bankgroups - 1));
            bankpergroup = GetRandomInt(0, (config_.banks_per_group - 1));
            row = GetRandomInt(0, (config_.rows - 1));
            column = GetRandomInt(0, (config_.actual_colums - 1));
            bl = GetRandomInt(0, config_.BL - 1);
            bit = GetRandomInt(0, (config_.bus_width - 1));

            rate = (uint16_t) (std::abs(GetNormalInt(0, 50)) + 2);
            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].vrt_size++;
            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].vrt.push_back(
                    std::make_pair(bit, rate));

            //std::cout <<"Low : " << bit << "   " << rate << "\n";
        }


        for (uint64_t i = 0; i < num_generate_mid; i++) {
            int channel, rank, bankgroup, bankpergroup, row, column, bl, bit;
            channel = GetRandomInt(0, (config_.channels - 1));
            rank = GetRandomInt(0, (config_.ranks - 1));
            bankgroup = GetRandomInt(0, (config_.bankgroups - 1));
            bankpergroup = GetRandomInt(0, (config_.banks_per_group - 1));
            row = GetRandomInt(0, (config_.rows - 1));
            column = GetRandomInt(0, (config_.actual_colums - 1));
            bl = GetRandomInt(0, config_.BL - 1);
            bit = GetRandomInt(0, (config_.bus_width - 1));

            rate = (uint16_t) GetRandomInt(1, 999);
            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].vrt_size++;
            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].vrt.push_back(
                    std::make_pair(bit, rate));

            //std::cout <<"Mid : " << bit << "   " << rate << "\n";
        }

        for (uint64_t i = 0; i < num_generate_high; i++) {
            int channel, rank, bankgroup, bankpergroup, row, column, bl, bit;
            channel = GetRandomInt(0, (config_.channels - 1));
            rank = GetRandomInt(0, (config_.ranks - 1));
            bankgroup = GetRandomInt(0, (config_.bankgroups - 1));
            bankpergroup = GetRandomInt(0, (config_.banks_per_group - 1));
            row = GetRandomInt(0, (config_.rows - 1));
            column = GetRandomInt(0, (config_.actual_colums - 1));
            bl = GetRandomInt(0, config_.BL - 1);
            bit = GetRandomInt(0, (config_.bus_width - 1));

            rate = (uint16_t) (999 - (std::abs(GetNormalInt(0, 50)) + 1));
            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].vrt_size++;
            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].vrt.push_back(
                    std::make_pair(bit, rate));

            //std::cout <<"High : " << bit << "   " << rate << "\n";
        }

        for (uint64_t i = 0; i < num_generate_high_high; i++) {
            int channel, rank, bankgroup, bankpergroup, row, column, bl, bit;
            channel = GetRandomInt(0, (config_.channels - 1));
            rank = GetRandomInt(0, (config_.ranks - 1));
            bankgroup = GetRandomInt(0, (config_.bankgroups - 1));
            bankpergroup = GetRandomInt(0, (config_.banks_per_group - 1));
            row = GetRandomInt(0, (config_.rows - 1));
            column = GetRandomInt(0, (config_.actual_colums - 1));
            bl = GetRandomInt(0, config_.BL - 1);
            bit = GetRandomInt(0, (config_.bus_width - 1));

            rate = (uint16_t) (999 - (std::abs(GetNormalInt(0, 1)) + 1));
            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].vrt_size++;
            fault_map_[channel][rank][bankgroup][bankpergroup][row][column][bl].vrt.push_back(
                    std::make_pair(bit, rate));

            //std::cout <<"High : " << bit << "   " << rate << "\n";
        }

    }
}