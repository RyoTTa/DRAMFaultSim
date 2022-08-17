#include "common.h"

namespace dramfaultsim {

    void AbruptExit(const std::string &file, int line) {
        std::cerr << "Exiting Abruptly - " << file << ":" << line << std::endl;
        std::exit(-1);
    }

    int LogBase2(int power_of_two) {
        int i = 0;
        while (power_of_two > 1) {
            power_of_two /= 2;
            i++;
        }
        return i;
    }

    void PrintAddress(Address addr){
        std::cout << "Channel:   " << addr.channel << std::endl;
        std::cout << "Rank:      " << addr.rank << std::endl;
        std::cout << "Bankgroup: " << addr.bankgroup << std::endl;
        std::cout << "Bank:      " << addr.bank << std::endl;
        std::cout << "Row:       " << addr.row << std::endl;
        std::cout << "Column:    " << addr.column << std::endl;

    }

}
