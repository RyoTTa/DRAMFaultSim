//
// Created by ryotta205 on 8/17/22.
//
#include "generator.h"

namespace dramfaultsim{

    bool RandomGenerator::AccessMemory() {
        Generator::AccessMemory();

        gen_addr = gen();
        std::cout << "Accessing Memory: 0x" << std::hex << gen_addr << std::dec << std::endl;

        if (config_.num_request == 0)
            return false;
        else if (num_executed_request >= config_.num_request)
            return true;
        else
            return false;

    }
}