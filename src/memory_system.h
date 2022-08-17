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
        MemorySystem(const std::string &config_file, const std::string &output_dir)
        : config_(new Config(config_file, output_dir)){};
        ~MemorySystem(){};

    protected:
        Config *config_;
    };
}

#endif //DRAMFAULTSIM_MEMORY_SYSTEM_H
