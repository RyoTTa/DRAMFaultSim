//
// Created by ryotta205 on 8/19/22.
//
#include "faultmodel.h"

namespace dramfaultsim{
    NaiveFaultModel::NaiveFaultModel(Config config, uint64_t ******data_block) : FaultModel(config, data_block){
#ifdef TEST_MODE
        std::cout << "NaiveFaultModel Constructor" << std::endl;
#endif
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
                                fault_map_[i][j][k][q][e][f].hardfault = 0xffffffffffffffff;
                            }
                        }
                    }
                }
            }
        }
    }
}