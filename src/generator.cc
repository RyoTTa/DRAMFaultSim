//
// Created by ryotta205 on 8/17/22.
//
#include "generator.h"
#include <bitset>

namespace dramfaultsim {
    RandomGenerator::RandomGenerator(Config &config)
            : Generator(config) {
        if (config_.memory_system == "NaiveMemorySystem") {
            memory_system_ = new NaiveMemorySystem(config_);
        }

    }

    RandomGenerator::~RandomGenerator() {
#ifdef TEST_MODE
        std::cout << "RandomGenerator destructor" << std::endl;
#endif

        delete memory_system_;
    }

    bool RandomGenerator::AccessMemory() {
        Generator::AccessMemory();

#ifndef TEST_MODE
        std::mt19937_64 gen(rd());
#endif

        gen_addr = gen();
        gen_addr = gen_addr & config_.effective_addr_mask;

        gen_data = gen();

        is_write = (gen() % 5 == 0);


#ifdef TEST_MODE
        std::cout << "Generate Memory(MASK)  : 0x" << std::hex << gen_addr << std::dec << (is_write ? "  WRITE" : "  READ")<< std::endl;
        std::cout << "Generate Data          : 0x" << std::hex << gen_data << std::dec << std::endl;
#endif

        //TEST
        Address recv_addr_ = config_.AddressMapping(gen_addr);

        int recv_addr_channel = recv_addr_.channel;
        int recv_addr_rank = recv_addr_.rank;
        int recv_addr_bankgroup = recv_addr_.bankgroup;
        int recv_addr_bank = recv_addr_.bank;
        int recv_addr_row = recv_addr_.row;
        int recv_addr_column = recv_addr_.column;

        std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
        std::cout << std::dec << recv_addr_channel << " " << recv_addr_rank << " " << recv_addr_bankgroup << " " << recv_addr_bank
                  << " " << recv_addr_row << " " << recv_addr_column << std::endl;
        std::cout << std::bitset<64>(recv_addr_.hex_addr) << std::dec << std::endl;

        uint64_t recv_addr_2 = config_.ReverseAddressMapping(recv_addr_channel, recv_addr_rank, recv_addr_bankgroup,
                                                             recv_addr_bank, recv_addr_row, recv_addr_column);

        Address recv_addr_a = config_.AddressMapping(recv_addr_2);

        recv_addr_channel = recv_addr_a.channel;
        recv_addr_rank = recv_addr_a.rank;
        recv_addr_bankgroup = recv_addr_a.bankgroup;
        recv_addr_bank = recv_addr_a.bank;
        recv_addr_row = recv_addr_a.row;
        recv_addr_column = recv_addr_a.column;

        std::cout << recv_addr_channel << " " << recv_addr_rank << " " << recv_addr_bankgroup << " " << recv_addr_bank
                  << " " << recv_addr_row << " " << recv_addr_column << std::endl;
        std::cout << std::bitset<64>(recv_addr_a.hex_addr) << std::dec << std::endl;

        std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;



        //TEST
        memory_system_->RecvRequest(gen_addr, is_write, gen_data);



        if (config_.num_request == 0)
            return false;
        if (num_executed_request >= config_.num_request)
            return true;
        else
            return false;

    }
}