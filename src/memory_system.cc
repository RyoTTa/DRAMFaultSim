//
// Created by ryotta205 on 8/17/22.
//
#include "memory_system.h"
#include <bitset>

namespace dramfaultsim {
    void MemorySystem::FaultData(uint64_t *data) {
        for (int i = 0; i < config_.BL; i++) {
            fault_data[i] = data[i] ^ fault_mask[i];
        }
    }

    NaiveMemorySystem::NaiveMemorySystem(Config &config, Stat &stat) : MemorySystem(config, stat) {
#ifndef TEST_MODE
        std::mt19937_64 gen(rd());
#endif
#ifdef TEST_MODE
        std::cout << "NaiveMemorySystem" << std::endl;
#endif
        //data_block_[Channel][Rank][BankGourp][Bank][Row][Col]
        data_block_ = new uint64_t ******[config_.channels];

        for (int i = 0; i < config_.channels; i++) {
            data_block_[i] = new uint64_t *****[config_.ranks];
            for (int j = 0; j < config_.ranks; j++) {
                data_block_[i][j] = new uint64_t ****[config_.bankgroups];
                for (int k = 0; k < config_.bankgroups; k++) {
                    data_block_[i][j][k] = new uint64_t ***[config_.banks_per_group];
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        data_block_[i][j][k][q] = new uint64_t **[config_.rows];
                        for (int e = 0; e < config_.rows; e++) {
                            data_block_[i][j][k][q][e] = new uint64_t *[config_.actual_colums];
                            for (int w = 0; w < config_.actual_colums; w++) {
                                data_block_[i][j][k][q][e][w] = new uint64_t[config_.BL];
                                if (config_.data_pattern_str == "Random") {
                                    //Generate Random Data Pattern
                                    for (int f = 0; f < config_.BL; f++) {
                                        data_block_[i][j][k][q][e][w][f] = gen();
                                    }
                                } else {
                                    //Generate Configuration Data Pattern
                                    for (int f = 0; f < config_.BL; f++) {
                                        data_block_[i][j][k][q][e][w][f] = config_.data_pattern;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (config_.fault_model == "NaiveFaultModel") {
            faultmodel_ = new NaiveFaultModel(config_, data_block_, stat_);
        }
        fault_data = new uint64_t[config_.BL];
    }

    NaiveMemorySystem::~NaiveMemorySystem() {
#ifdef TEST_MODE
        std::cout << "NaiveMemorySystem destructor" << std::endl;
#endif
        delete faultmodel_;

        for (int i = 0; i < config_.channels; i++) {
            for (int j = 0; j < config_.ranks; j++) {
                for (int k = 0; k < config_.bankgroups; k++) {
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        for (int e = 0; e < config_.rows; e++) {
                            for (int w = 0; w < config_.actual_colums; w++) {
                                delete[] data_block_[i][j][k][q][e][w];
                            }
                            delete[] data_block_[i][j][k][q][e];
                        }
                        delete[] data_block_[i][j][k][q];
                    }
                    delete[] data_block_[i][j][k];
                }
                delete[] data_block_[i][j];
            }
            delete[] data_block_[i];
        }
        delete[] data_block_;
    }

    void NaiveMemorySystem::RecvRequest(uint64_t addr, bool is_write, uint64_t *data) {
        /*
#ifdef TEST_MODE
        std::cout << "Recv Request  : 0x" << std::hex << addr << std::dec << (is_write ? "  WRITE" : "  READ")<< std::endl;
        for (int i=0; i < config_.BL; i++){
            std::cout << "Recv Data    "<< i <<": 0x" << std::hex << data[i] << std::dec << std::endl;
        }
#endif
         */
        //Translate Recv Address
        SetRecvAddress(addr);

        if (is_write == true) {
            Write(data);
        } else {
            Read(data);
        }

        fault_mask = faultmodel_->ErrorInjection(addr);
        MemorySystem::FaultData(data);

        int temp = 0;
        for (int i = 0; i < config_.BL; i++) {
            if (fault_mask[i] != (uint64_t) 0) {
                temp = 1;
            }
        }

        if (temp) {
            stat_.fault_request_num++;
            std::cout << "Request  : 0x" << std::hex << recv_addr_.hex_addr << std::dec << std::endl;
            std::cout << "Request  :   " << recv_addr_channel << recv_addr_rank << recv_addr_bankgroup
            << recv_addr_bank << recv_addr_row << recv_addr_column << std::endl;
            std::cout << "Request  :   " << recv_addr_.channel << recv_addr_.rank << recv_addr_.bankgroup
                      << recv_addr_.bank << recv_addr_.row << recv_addr_.column << std::endl;
            for (int i=0; i < config_.BL; i++){
                std::cout << "Correct Data   "<< i <<": 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank]\
            [recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i] << std::dec << std::endl;
            }
            for (int i=0; i < config_.BL; i++){
                std::cout << "Fault Mask     "<< i <<": 0x" << std::hex << fault_mask[i] << std::dec << std::endl;
            }
            for (int i=0; i < config_.BL; i++){
                std::cout << "Fault Data   "<< i <<": 0x" << std::hex << fault_data[i] << std::dec << std::endl;
            }
        } else {
            stat_.correct_request_num++;
        }

        return;
    }

    void NaiveMemorySystem::Read(uint64_t *data) {
        /*
#ifdef TEST_MODE
        std::cout << "Read Request  : 0x" << std::hex << recv_addr_.hex_addr << std::dec << std::endl;
        for (int i=0; i < config_.BL; i++){
            std::cout << "Read Data Before   "<< i <<": 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank]\
            [recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i] << std::dec << std::endl;
        }
#endif
         */

        for (int i = 0; i < config_.BL; i++) {
            data_block_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i] = data[i];
        }
        /*
#ifdef TEST_MODE
        for (int i=0; i < config_.BL; i++){
            std::cout << "Read Data After   "<< i <<": 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank]\
            [recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i] << std::dec << std::endl;
        }
#endif
         */
        return;
    }

    void NaiveMemorySystem::Write(uint64_t *data) {
        /*
#ifdef TEST_MODE
        std::cout << "Write Request  : 0x" << std::hex << recv_addr_.hex_addr << std::dec << std::endl;
        for (int i=0; i < config_.BL; i++){
            std::cout << "Write Data Before   "<< i <<": 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank]\
            [recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i] << std::dec << std::endl;
        }
#endif
         */

        for (int i = 0; i < config_.BL; i++) {
            data_block_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i] = data[i];
        }
        /*
#ifdef TEST_MODE
        for (int i=0; i < config_.BL; i++){
            std::cout << "Write Data After   "<< i <<": 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank]\
            [recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column][i] << std::dec << std::endl;
        }
#endif
         */
        return;
    }
}