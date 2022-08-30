#ifndef RAMFAULTSIM_COMMON_H
#define RAMFAULTSIM_COMMON_H

#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <vector>


namespace dramfaultsim{


struct Address {
    Address()
            : channel(-1), rank(-1), bankgroup(-1), bank(-1), row(-1), column(-1) {}

    Address(int channel, int rank, int bankgroup, int bank, int row, int column, uint64_t hex_addr)
            : channel(channel),
              rank(rank),
              bankgroup(bankgroup),
              bank(bank),
              row(row),
              column(column),
              hex_addr(hex_addr) {}

    Address(const Address &addr)
            : channel(addr.channel),
              rank(addr.rank),
              bankgroup(addr.bankgroup),
              bank(addr.bank),
              row(addr.row),
              column(addr.column),
              hex_addr(addr.hex_addr) {}

    int channel;
    int rank;
    int bankgroup;
    int bank;
    int row;
    int column;
    uint64_t hex_addr;
};

typedef struct FaultStruct {
    uint64_t hardfault;
    uint64_t vrt_low;
    uint64_t vrt_mid;
    uint64_t vrt_high;

} FaultStruct;


void AbruptExit(const std::string &file, int line);

int LogBase2(int power_of_two);

void PrintAddress(Address addr);

}
#endif //RAMFAULTSIM_COMMON_H