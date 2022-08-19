//
// Created by ryotta205 on 8/19/22.
//
#include "common.h"
#include "configuration.h"

#ifndef DRAMFAULTSIM_FAULTMODEL_H
#define DRAMFAULTSIM_FAULTMODEL_H
namespace dramfaultsim {

    typedef struct FaultStruct {
        uint64_t hardfault;

    } FaultStruct;

    class FaultModel {
    public:
        FaultModel(Config config, uint64_t ******data_block)
                : config_(config), data_block_(data_block) {}

    protected:
        Config config_;
        Address addr_;
        uint64_t ******data_block_;
    };

    class NaiveFaultModel : public FaultModel {
    public:
        NaiveFaultModel(Config config, uint64_t ******data_block);

        ~NaiveFaultModel();

    protected:

    private:
        FaultStruct ******fault_map_;
        std::random_device rd;
#ifdef TEST_MODE
        std::mt19937_64 gen;
#endif
    };

}
#endif //DRAMFAULTSIM_FAULTMODEL_H
