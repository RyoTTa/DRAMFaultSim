//
// Created by ryotta205 on 8/17/22.
//
#include "generator.h"

namespace dramfaultsim{

    void RandomGenerator::AccessMemory() {
        last_addr = gen();
        std::cout << "Accessing Memory: 0x" << std::hex << last_addr << std::dec << std::endl;
        //config_->PrintInfo();
    }
}