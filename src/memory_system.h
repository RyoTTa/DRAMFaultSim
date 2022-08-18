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
                : config_(config){}
        ~MemorySystem(){};

    protected:
        Config config_;
    };


    class MemorySystemNaive : public MemorySystem{
    public:
        MemorySystemNaive(Config config);

    protected:
        uint64_t ******DataBlock;
        uint64_t ******Channel;
        uint64_t *****Rank;
        uint64_t ****BankGroup;
        uint64_t ***Bank;
        uint64_t **Row;
        uint64_t *Column;

    private:
        std::random_device rd;
        uint64_t gen_addr;
#ifdef TEST_MODE
        std::mt19937_64 gen;
#endif
    };
}

#endif //DRAMFAULTSIM_MEMORY_SYSTEM_H
