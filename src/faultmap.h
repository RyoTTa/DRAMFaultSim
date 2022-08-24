//
// Created by ryotta205 on 8/24/22.
//

#ifndef DRAMFAULTSIM_FAULTMAP_H
#define DRAMFAULTSIM_FAULTMAP_H

#include "common.h"
#include "configuration.h"
#include <fstream>

namespace dramfaultsim {
    class FaultMapReader {
    public:
        FaultMapReader( Config &config, std::string path, FaultStruct ****** fault_map);

        ~FaultMapReader(){
            reader_.close();
        }

    private:
        Config &config_;
        std::string path_;
        std::ifstream reader_;
        FaultStruct ****** fault_map_;
    };

    class FaultMapWriter {
    public:
        FaultMapWriter(Config &config, std::string path, FaultStruct ****** fault_map);

        ~FaultMapWriter(){
            writer_.close();
        }

        bool Write();

    private:
        Config &config_;
        std::string path_;
        std::ofstream writer_;
        FaultStruct ****** fault_map_;
    };
}
#endif //DRAMFAULTSIM_FAULTMAP_H
