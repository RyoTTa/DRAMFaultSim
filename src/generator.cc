//
// Created by ryotta205 on 8/17/22.
//
#include "generator.h"

namespace dramfaultsim {
    RandomGenerator::RandomGenerator(Config &config)
    : Generator(config){
        if(config_.memory_system == "NaiveMemorySystem"){
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
        memory_system_->RecvRequest(gen_addr, is_write, gen_data);

        if (config_.num_request == 0)
            return false;
        if (num_executed_request >= config_.num_request)
            return true;
        else
            return false;

    }
}