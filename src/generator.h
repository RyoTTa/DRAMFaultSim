//
// Created by ryotta205 on 8/17/22.
//
#ifndef DRAMFAULTSIM_GENERATOR_H
#define DRAMFAULTSIM_GENERATOR_H

#include "common.h"
#include "configuration.h"
#include "memory_system.h"

namespace dramfaultsim {

    class Generator {
    public:
        Generator(const std::string &config_file, const std::string &out_dir, Config config)
                : config_(config), memory_system_(config) {
            num_executed_request = 0;
        }

        virtual bool AccessMemory(){
            num_executed_request += 1;
            return true;
        }

    protected:
        Config config_;
        MemorySystem memory_system_;
        uint64_t num_executed_request;

    };

    class RandomGenerator : public Generator {
    public:
        using Generator::Generator;

        bool AccessMemory() override;

    private:
        uint64_t gen_addr;
        //bool last_write = false;
        std::mt19937_64 gen;
        //bool get_next_ = true;
    };
}

#endif //DRAMFAULTSIM_GENERATOR_H
