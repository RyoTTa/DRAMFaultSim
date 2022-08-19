//
// Created by ryotta205 on 8/17/22.
//

#ifndef DRAMFAULTSIM_MEMORY_SYSTEM_H
#define DRAMFAULTSIM_MEMORY_SYSTEM_H

#include "common.h"
#include "configuration.h"

namespace dramfaultsim {

    class MemorySystem {
    public:
        MemorySystem(Config config)
                : config_(config) {}

        ~MemorySystem() {};

        virtual void RecvRequest(uint64_t addr, bool is_write, uint64_t data) = 0;

        void SetRecvAddress(uint64_t addr) {
            recv_addr = config_.AddressMapping(addr);

            recv_addr_channel = recv_addr.channel;
            recv_addr_rank = recv_addr.rank;
            recv_addr_bankgroup = recv_addr.bankgroup;
            recv_addr_bank = recv_addr.bank;
            recv_addr_row = recv_addr.row;
            recv_addr_column = recv_addr.column;
        }

        void Read(uint64_t data) {
#ifdef TEST_MODE
            std::cout << "Read Request      : 0x" << std::hex << recv_addr.hex_addr << std::dec << std::endl;
            std::cout << "Read Data Before  : 0x" << std::hex << DataBlock[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl;
#endif
            DataBlock[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] = data;
#ifdef TEST_MODE
            std::cout << "Read Data After   : 0x" << std::hex << DataBlock[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl << std::endl;
#endif
        }

        void Write(uint64_t data) {
#ifdef TEST_MODE
            std::cout << "Write Request     : 0x" << std::hex << recv_addr.hex_addr << std::dec << std::endl;
            std::cout << "Write Data Before : 0x" << std::hex << DataBlock[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl;
#endif
            DataBlock[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] = data;
#ifdef TEST_MODE
            std::cout << "Write Data After  : 0x" << std::hex << DataBlock[recv_addr_channel][recv_addr_rank][recv_addr_bankgroup][recv_addr_bank][recv_addr_row][recv_addr_column] << std::dec << std::endl << std::endl;
#endif
        }


    protected:
        Config config_;
        Address recv_addr;
        //DataBlock[Channel][Rank][BankGourp][Bank][Row][Col]
        uint64_t ******DataBlock;
        uint8_t recv_addr_channel;
        uint8_t recv_addr_rank;
        uint8_t recv_addr_bankgroup;
        uint8_t recv_addr_bank;
        uint8_t recv_addr_row;
        uint8_t recv_addr_column;

    };


    class MemorySystemNaive : public MemorySystem {
    public:
        MemorySystemNaive(Config config);

        ~MemorySystemNaive();

        void RecvRequest(uint64_t addr, bool is_write, uint64_t data) override;

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
