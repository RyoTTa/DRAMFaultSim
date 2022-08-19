//
// Created by ryotta205 on 8/17/22.
//
#ifndef DRAMFAULTSIM_GENERATOR_H
#define DRAMFAULTSIM_GENERATOR_H

#include "common.h"
#include "configuration.h"
#include "memory_system.h"
#include <random>

namespace dramfaultsim {

    class Generator {
    public:
        Generator(Config config)
                : config_(config), num_executed_request(0) {}

        virtual bool AccessMemory() {
            num_executed_request++;
            return true;
        }

    protected:
        Config config_;
        MemorySystem *memory_system_;
        uint64_t num_executed_request;

    };

    class RandomGenerator : public Generator {
    public:
        RandomGenerator(Config config);

        bool AccessMemory() override;

    private:
        std::random_device rd;
        uint64_t gen_addr;
        uint64_t gen_data;
        bool is_write;

#ifdef TEST_MODE
        std::mt19937_64 gen;
#endif
    };
}

#endif //DRAMFAULTSIM_GENERATOR_H
