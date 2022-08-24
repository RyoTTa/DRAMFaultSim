//
// Created by ryotta205 on 8/19/22.
//
#include "common.h"
#include "configuration.h"
#include "faultmap.h"

#ifndef DRAMFAULTSIM_FAULTMODEL_H
#define DRAMFAULTSIM_FAULTMODEL_H
namespace dramfaultsim {


    class FaultModel {
    public:
        FaultModel(Config &config, uint64_t ******data_block)
                : config_(config), data_block_(data_block) {};
        virtual ~FaultModel(){};

        virtual uint64_t ErrorInjection(uint64_t addr) = 0;
        virtual uint64_t HardFaultError() = 0;
        //virtual uint64_t VRTError() = 0;

        double GetRandomDobule(double low, double high){
            std::uniform_real_distribution<> dist(low, high);
            std::mt19937_64 gen(rd());

            //std::cout << dist(gen) << std::endl;
            return dist(gen);
        }

        int GetRandomInt(int low, int high){
            std::uniform_int_distribution<> dist(low, high);
            std::mt19937_64 gen(rd());

            //std::cout << dist(gen) << std::endl;
            return dist(gen);
        }

    protected:
        Config &config_;
        Address recv_addr_;
        uint64_t ******data_block_;

        uint8_t recv_addr_channel;
        uint8_t recv_addr_rank;
        uint8_t recv_addr_bankgroup;
        uint8_t recv_addr_bank;
        uint8_t recv_addr_row;
        uint8_t recv_addr_column;

        void SetRecvAddress(uint64_t addr) {
            recv_addr_ = config_.AddressMapping(addr);

            recv_addr_channel = recv_addr_.channel;
            recv_addr_rank = recv_addr_.rank;
            recv_addr_bankgroup = recv_addr_.bankgroup;
            recv_addr_bank = recv_addr_.bank;
            recv_addr_row = recv_addr_.row;
            recv_addr_column = recv_addr_.column;

            return;
        }
    private:
        std::random_device rd;
#ifdef TEST_MODE
        std::mt19937_64 gen;
#endif
    };


    class NaiveFaultModel : public FaultModel {
    public:
        NaiveFaultModel(Config &config, uint64_t ******data_block);

        ~NaiveFaultModel() override;

        uint64_t ErrorInjection(uint64_t addr) override;

        uint64_t HardFaultError() override;

        //uint64_t VRTError() override;

        void HardFaultGenerator();

        void HardFaultGeneratorThread(uint64_t num_generate);

        void VRTErrorGenerator();

        void VRTErrorGeneratorThread(uint64_t num_generate_low, uint64_t num_generate_mid, uint64_t num_generate_high);

    protected:

    private:
        FaultStruct ******fault_map_;
        std::random_device rd;

        uint64_t ErrorMask;
        uint64_t num_all_cell;
        uint64_t num_hard_fault_cell;
        uint64_t num_vrt_fault_cell;
        uint64_t num_vrt_fault_low_cell;
        uint64_t num_vrt_fault_mid_cell;
        uint64_t num_vrt_fault_high_cell;

#ifdef TEST_MODE
        std::mt19937_64 gen;
#endif
    };

}
#endif //DRAMFAULTSIM_FAULTMODEL_H
