//
// Created by ryotta205 on 8/17/22.
//
#include "memory_system.h"
#include <bitset>

namespace dramfaultsim {
    uint64_t MemorySystem::FaultData(uint64_t data) {
        return data ^ fault_mask;
    }

    NaiveMemorySystem::NaiveMemorySystem(Config &config, Stat &stat) : MemorySystem(config, stat) {
#ifndef TEST_MODE
        std::mt19937_64 gen(rd());
#endif
        //data_block_[Channel][Rank][BankGourp][Bank][Row][Col]
        data_block_ = new uint64_t *****[config_.channels];

        for (int i = 0; i < config_.channels; i++) {
            data_block_[i] = new uint64_t ****[config_.ranks];
            for (int j = 0; j < config_.ranks; j++) {
                data_block_[i][j] = new uint64_t ***[config_.bankgroups];
                for (int k = 0; k < config_.bankgroups; k++) {
                    data_block_[i][j][k] = new uint64_t **[config_.banks_per_group];
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        data_block_[i][j][k][q] = new uint64_t *[config_.rows];
                        for (int e = 0; e < config_.rows; e++) {
                            data_block_[i][j][k][q][e] = new uint64_t[config_.columns];
                            if (config_.data_pattern_str == "Random") {
                                //Generate Random Data Pattern
                                for (int f = 0; f < config_.columns; f++) {
                                    data_block_[i][j][k][q][e][f] = gen();
                                }
                            } else {
                                //Generate Configuration Data Pattern
                                for (int f = 0; f < config_.columns; f++) {
                                    data_block_[i][j][k][q][e][f] = config_.data_pattern;
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

    void NaiveMemorySystem::RecvRequest(uint64_t addr, bool is_write, uint64_t data) {
#ifdef TEST_MODE
        std::cout << "Recv Request  : 0x" << std::hex << addr << std::dec << (is_write ? "  WRITE" : "  READ")\
        << std::endl;
        std::cout << "Recv Data     : 0x" << std::hex << data << std::dec << std::endl;
#endif
        //Translate Recv Address
        SetRecvAddress(addr);

        if (is_write == true) {
            Write(data);
        } else {
            Read(data);
        }

        fault_mask = faultmodel_->ErrorInjection(addr);
        fault_data = MemorySystem::FaultData(data);

        if (fault_mask != (uint64_t) 0) {
            stat_.fault_request_num++;
        }else{
            stat_.correct_request_num++;
        }
#ifdef TEST_MODE
        if(fault_mask != (uint64_t)0){
            std::cout << "CorrectData: 0x" << std::hex << data << std::dec << std::endl;
            std::cout << "CorrectData: 0b" << std::bitset<64>(data) << std::endl << std::endl;

            std::cout << "ErrorMask  : 0x" << std::hex << fault_mask << std::dec << std::endl;
            std::cout << "ErrorMask  : 0b" << std::bitset<64>(fault_mask) << std::endl << std::endl;

            std::cout << "ErrorData  : 0x" << std::hex << fault_data << std::dec << std::endl;
            std::cout << "ErrorData  : 0b" << std::bitset<64>(fault_data) << std::endl << std::endl;
        }
#endif
        return;
    }

    void NaiveMemorySystem::Read(uint64_t data) {
#ifdef TEST_MODE
        std::cout << "Read Request      : 0x" << std::hex << recv_addr_.hex_addr << std::dec << std::endl;
        std::cout << "Read Data Before  : 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank]\
            [recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl;
#endif

        data_block_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row]\
[recv_addr_column] = data;

#ifdef TEST_MODE
        std::cout << "Read Data After   : 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank]\
        [recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl << std::endl;
#endif
        return;
    }

    void NaiveMemorySystem::Write(uint64_t data) {
#ifdef TEST_MODE
        std::cout << "Write Request     : 0x" << std::hex << recv_addr_.hex_addr << std::dec << std::endl;
        std::cout << "Write Data Before : 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank]\
            [recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl;
#endif

        data_block_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row]\
[recv_addr_column] = data;

#ifdef TEST_MODE
        std::cout << "Write Data After  : 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank]\
        [recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl << std::endl;
#endif
        return;
    }
}