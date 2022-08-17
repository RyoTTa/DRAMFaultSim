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
        Generator(const std::string &config_file, const std::string &out_dir)
                : config_(new Config(config_file, out_dir)), memory_system_(config_file, out_dir) {}

        virtual void AccessMemory() = 0;

    protected:
        MemorySystem memory_system_;
        Config *config_;

    };

    class RandomGenerator : public Generator {
    public:
        using Generator::Generator;

        void AccessMemory() override;

    private:
        uint64_t last_addr;
        bool last_write = false;
        std::mt19937_64 gen;
        bool get_next_ = true;
    };
}

#endif //DRAMFAULTSIM_GENERATOR_H
