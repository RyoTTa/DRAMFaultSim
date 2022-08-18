//
// Created by ryotta205 on 8/17/22.
//
#include "memory_system.h"

namespace dramfaultsim {

    MemorySystemNaive::MemorySystemNaive(Config config) : MemorySystem(config) {

#ifndef TEST_MODE
        std::mt19937_64 gen(rd());
#endif
        Channel = new uint64_t *****[config_.channels];
        for (int i = 0; i < config_.channels; i++) {
            Channel[i] = new uint64_t ****[config_.ranks];
            for (int j = 0; j < config_.ranks; j++) {
                Channel[i][j] = new uint64_t ***[config_.bankgroups];
                for (int k = 0; k < config_.bankgroups; k++) {
                    Channel[i][j][k] = new uint64_t **[config_.banks_per_group];
                    for (int q = 0; q < config_.banks_per_group; q++) {
                        Channel[i][j][k][q] = new uint64_t *[config_.rows];
                        for (int e = 0; e < config_.rows; e++) {
                            Channel[i][j][k][q][e] = new uint64_t[config_.columns];
                            for (int f = 0; f < config_.columns; f++) {
                                Channel[i][j][k][q][e][f] = gen();
                            }
                        }
                    }
                }
            }
        }

        DataBlock = Channel;

        std::cout << DataBlock[0][0][0][0][0][0] << std::endl;
    }
}