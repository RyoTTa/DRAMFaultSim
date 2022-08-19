//
// Created by ryotta205 on 8/17/22.
//
#include "memory_system.h"

namespace dramfaultsim {

    MemorySystemNaive::MemorySystemNaive(Config config) : MemorySystem(config) {
#ifndef TEST_MODE
        std::mt19937_64 gen(rd());
#endif
        //DataBlock[Channel][Rank][BankGourp][Bank][Row][Col]
        DataBlock = new uint64_t *****[config_.channels];

        for (int i = 0; i < config_.channels; i++) {
            DataBlock[i] = new uint64_t ****[config_.ranks];
            for (int j = 0; j < config_.ranks; j++) {
                DataBlock[i][j] = new uint64_t ***[config_.bankgroups];
                for (int k = 0; k < config_.bankgroups; k++) {
                    DataBlock[i][j][k] = new uint64_t **[config_.banks_per_group];
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        DataBlock[i][j][k][q] = new uint64_t *[config_.rows];
                        for (int e = 0; e < config_.rows; e++) {
                            DataBlock[i][j][k][q][e] = new uint64_t[config_.columns];
                            for (int f = 0; f < config_.columns; f++) {
                                DataBlock[i][j][k][q][e][f] = gen();
                            }
                        }
                    }
                }
            }
        }

        std::cout << DataBlock[0][0][0][0][0][0] << std::endl;
    }

    MemorySystemNaive::~MemorySystemNaive(){

        for (int i = 0; i < config_.channels; i++) {
            for (int j = 0; j < config_.ranks; j++) {
                for (int k = 0; k < config_.bankgroups; k++) {
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        for (int e = 0; e < config_.rows; e++) {
                            delete[] DataBlock[i][j][k][q][e];
                        }
                        delete[] DataBlock[i][j][k][q];
                    }
                    delete[] DataBlock[i][j][k];
                }
                delete[] DataBlock[i][j];
            }
            delete[] DataBlock[i];
        }
        delete[] DataBlock;
    }

    void MemorySystemNaive::RecvRequest(uint64_t addr, bool is_write, uint64_t data) {
#ifdef TEST_MODE
        std::cout << "Recv Request  : 0x" << std::hex << addr << std::dec << (is_write ? "  WRITE" : "  READ")<< std::endl;
        std::cout << "Recv Data     : 0x" << std::hex << data << std::dec << std::endl;
#endif
        //Translate Recv Address
        SetRecvAddress(addr);

        if (is_write == true){
            Write(data);
        }else{
            Read(data);
        }

    }
}