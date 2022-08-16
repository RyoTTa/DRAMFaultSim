#include <stdint.h>
#include <iostream>
#include <vector>

namespace dramfaultsim {

struct Address{
    Address()
        : channel(-1), rank(-1), bankgroup(-1), bank(-1), row(-1), column(-1) {}
    Address(int channel, int rank, int bankgroup, int bank, int row, int column)
        : channel(channel),
        rank(rank),
        bankgroup(bankgroup),
        bank(bank),
        row(row),
        column(column) {}
    Address(const Address& addr)
        : channel(addr.channel),
        rank(addr.rank),
        bankgroup(addr.bankgroup),
        bank(addr.bank),
        row(addr.row),
        column(addr.column) {}
    int channel;
    int rank;
    int bankgroup;
    int bank;
    int row;
    int column;
};









}