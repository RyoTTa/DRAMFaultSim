//
// Created by ryotta205 on 8/17/22.
//
#include "generator.h"

namespace dramfaultsim {

    bool RandomGenerator::AccessMemory() {
        Generator::AccessMemory();
        memory_system_ = new MemorySystemNaive(config_);
#ifndef TEST_MODE
        std::mt19937_64 gen(rd());
#endif


        gen_addr = gen();
        //uint64_t gen_addr_test = gen_addr;
        gen_addr = gen_addr & config_.effective_addr_mask;
        is_write = (gen() % 5 == 0);


#ifdef TEST_MODE
        std::cout << "Generate Memory(MASK)  : 0x" << std::hex << gen_addr << std::dec << (is_write ? "  WRITE" : "  READ")<< std::endl;
        std::cout << "PARSING DATA" << std::endl;
        PrintAddress(config_.AddressMapping(gen_addr));
        //std::cout << "Generate Memory(Un-MASK: 0x" << std::hex << gen_addr_test << std::dec << (is_write ? "  WRITE" : "  READ")<< std::endl;
        //std::cout << "PARSING DATA" << std::endl;
        //PrintAddress(config_.AddressMapping(gen_addr_test));
#endif

        if (config_.num_request == 0)
            return false;
        if (num_executed_request >= config_.num_request)
            return true;
        else
            return false;

    }
}