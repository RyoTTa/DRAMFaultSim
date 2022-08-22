//
// Created by ryotta205 on 8/17/22.
//

#ifndef DRAMFAULTSIM_MEMORY_SYSTEM_H
#define DRAMFAULTSIM_MEMORY_SYSTEM_H

#include "common.h"
#include "configuration.h"
#include "faultmodel.h"

namespace dramfaultsim {

    class MemorySystem {
    public:
        MemorySystem(Config config)
                : config_(config) {}

        virtual ~MemorySystem() {};

        virtual void RecvRequest(uint64_t addr, bool is_write, uint64_t data) = 0;
        virtual void Read(uint64_t data) = 0;
        virtual void Write(uint64_t data) = 0;
        uint64_t FaultData(uint64_t data);

    protected:
        Config config_;
        Address recv_addr_;
        FaultModel *faultmodel_;

        //data_block_[Channel][Rank][BankGourp][Bank][Row][Col]
        uint64_t ******data_block_;
        uint8_t recv_addr_channel;
        uint8_t recv_addr_rank;
        uint8_t recv_addr_bankgroup;
        uint8_t recv_addr_bank;
        uint8_t recv_addr_row;
        uint8_t recv_addr_column;

        //For FaultModel Return Value
        uint64_t fault_mask;
        uint64_t fault_data;

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

    };

    class NaiveMemorySystem : public MemorySystem {
    public:
        NaiveMemorySystem(Config config);

        ~NaiveMemorySystem();

        void RecvRequest(uint64_t addr, bool is_write, uint64_t data) override;
        void Read(uint64_t data) override;
        void Write(uint64_t data) override;

    protected:


    private:
        std::random_device rd;
        uint64_t gen_addr;
#ifdef TEST_MODE
        std::mt19937_64 gen;
#endif


    };
}

#endif //DRAMFAULTSIM_MEMORY_SYSTEM_H
