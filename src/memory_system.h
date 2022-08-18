//
// Created by ryotta205 on 8/17/22.
//

#ifndef DRAMFAULTSIM_MEMORY_SYSTEM_H
#define DRAMFAULTSIM_MEMORY_SYSTEM_H
#include "common.h"
#include "configuration.h"

namespace dramfaultsim{

    class MemorySystem{
    public:
        MemorySystem(Config config)
        : config_(config){};
        ~MemorySystem(){};

    protected:
        Config config_;
    };
}

#endif //DRAMFAULTSIM_MEMORY_SYSTEM_H
