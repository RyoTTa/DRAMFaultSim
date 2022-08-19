//
// Created by ryotta205 on 8/17/22.
//
#include "memory_system.h"

namespace dramfaultsim {

    NaiveMemorySystem::NaiveMemorySystem(Config config) : MemorySystem(config) {
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
                            for (int f = 0; f < config_.columns; f++) {
                                data_block_[i][j][k][q][e][f] = gen();
                            }
                        }
                    }
                }
            }
        }
        if (config_.fault_model == "NaiveFaultModel") {
            faultmodel_ = new NaiveFaultModel(config_, data_block_);
        }
    }

    NaiveMemorySystem::~NaiveMemorySystem() {

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
        std::cout << "Recv Request  : 0x" << std::hex << addr << std::dec << (is_write ? "  WRITE" : "  READ")<< std::endl;
        std::cout << "Recv Data     : 0x" << std::hex << data << std::dec << std::endl;
#endif
        //Translate Recv Address
        SetRecvAddress(addr);

        if (is_write == true) {
            Write(data);
        } else {
            Read(data);
        }

    }

    void NaiveMemorySystem::Read(uint64_t data) {
#ifdef TEST_MODE
        std::cout << "Read Request      : 0x" << std::hex << recv_addr_.hex_addr << std::dec << std::endl;
            std::cout << "Read Data Before  : 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl;
#endif

        data_block_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] = data;

#ifdef TEST_MODE
        std::cout << "Read Data After   : 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl << std::endl;
#endif
    }

    void NaiveMemorySystem::Write(uint64_t data) {
#ifdef TEST_MODE
        std::cout << "Write Request     : 0x" << std::hex << recv_addr_.hex_addr << std::dec << std::endl;
            std::cout << "Write Data Before : 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl;
#endif

        data_block_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] = data;

#ifdef TEST_MODE
        std::cout << "Write Data After  : 0x" << std::hex << data_block_[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl << std::endl;
#endif
    }
}