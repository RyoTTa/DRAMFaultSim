//
// Created by ryotta205 on 8/17/22.
//
#include "configuration.h"

#include <vector>
#include <bitset>

namespace dramfaultsim {

    Config::Config(std::string config_file, std::string out_dir, uint64_t request)
            : output_dir(out_dir), num_request(request), reader_(new INIReader(config_file)) {
        if (reader_->ParseError() < 0) {
            std::cerr << "Can't load config file - " << config_file << std::endl;
            AbruptExit(__FILE__, __LINE__);
        }

        // The initialization of the parameters has to be strictly in this order
        // because of internal dependencies
        InitSystemParams();
        InitDRAMParams();
        InitFaultModelParams();
        CalculateSize();
        SetAddressMapping();

        delete (reader_);
    }

    Address Config::AddressMapping(uint64_t hex_addr) const {
        uint64_t origin_hex_addr = hex_addr;
        hex_addr >>= shift_bits;
        int channel = (hex_addr >> ch_pos) & ch_mask;
        int rank = (hex_addr >> ra_pos) & ra_mask;
        int bg = (hex_addr >> bg_pos) & bg_mask;
        int ba = (hex_addr >> ba_pos) & ba_mask;
        int ro = (hex_addr >> ro_pos) & ro_mask;
        int co = (hex_addr >> co_pos) & co_mask;
        return Address(channel, rank, bg, ba, ro, co, origin_hex_addr);
    }

    int Config::GetInteger(const std::string &sec, const std::string &opt,
                           int default_val) const {
        return static_cast<int>(reader_->GetInteger(sec, opt, default_val));
    }

    double Config::GetDouble(const std::string &sec, const std::string &opt,
                           double default_val) const {
        return static_cast<double>(reader_->GetReal(sec, opt, default_val));
    }

    void Config::InitSystemParams() {
        const auto &reader = *reader_;
        channel_size = GetInteger("system", "channel_size", 1024);
        channels = GetInteger("system", "channels", 1);
        bus_width = GetInteger("system", "bus_width", 64);
        address_mapping = reader.Get("system", "address_mapping", "chrobabgraco");
        memory_system = reader.Get("system", "memory_system", "NaiveMemorySystem");
        fault_model = reader.Get("system", "fault_model", "NaiveFaultModel");
        thread_model = reader.Get("system", "thread", "SingleThread");
        return;
    }

    void Config::InitDRAMParams() {
        const auto &reader = *reader_;
        bankgroups = GetInteger("dram_structure", "bankgroups", 2);
        banks_per_group = GetInteger("dram_structure", "banks_per_group", 2);
        bool bankgroup_enable =
                reader.GetBoolean("dram_structure", "bankgroup_enable", true);
        // GDDR5/6 can chose to enable/disable bankgroups
        if (!bankgroup_enable) {  // aggregating all banks to one group
            banks_per_group *= bankgroups;
            bankgroups = 1;
        }
        banks = bankgroups * banks_per_group;
        rows = GetInteger("dram_structure", "rows", 1 << 16);
        columns = GetInteger("dram_structure", "columns", 1 << 10);
        device_width = GetInteger("dram_structure", "device_width", 8);
        BL = GetInteger("dram_structure", "BL", 8);

        return;
    }

    void Config::InitFaultModelParams() {
        hard_fault_rate = GetDouble("fault_probability", "hard_fault_rate", 0.001);

        return;
    }

    void Config::CalculateSize() {
        // calculate rank and re-calculate channel_size
        devices_per_rank = bus_width / device_width;
        int page_size = columns * device_width / 8;  // page size in bytes
        int megs_per_bank = page_size * (rows / 1024) / 1024;
        int megs_per_rank = megs_per_bank * banks * devices_per_rank;

        if (megs_per_rank > channel_size) {
            std::cout << "WARNING: Cannot create memory system of size "
                      << channel_size
                      << "MB with given device choice! Using default size "
                      << megs_per_rank << " instead!" << std::endl;
            ranks = 1;
            channel_size = megs_per_rank;
        } else {
            ranks = channel_size / megs_per_rank;
            channel_size = ranks * megs_per_rank;
        }
        effective_addr_mask = ((uint64_t)1 << (LogBase2(channel_size) + LogBase2(1024) + LogBase2(1024))) - 1;
#ifdef TEST_MODE
        std::cout << "####TEST_MODE OUTPUT" << std::endl;
        std::cout << "####CalculateSize in Configuration" << std::endl;
        std::cout << "Device Per Rank: " << devices_per_rank << std::endl;
        std::cout << "Page Size: " << page_size << " B" << std::endl;
        std::cout << "Megabyte Per Bank: " << megs_per_bank << " MB" << std::endl;
        std::cout << "Megabyte Per Rank: " << megs_per_rank << " MB, "
                  << megs_per_rank / 1024 << " GB" << std::endl;
        std::cout << "Channel Size: " << channel_size << " MB, "
                  << channel_size / 1024 << " GB" << std::endl;
        std::cout << "Effective Addr Mask: " << effective_addr_mask << std::endl;
        std::cout << "Effective Addr Mask: " << std::bitset<64>(effective_addr_mask) << std::endl << std::endl;
#endif  // TEST_MODE
        return;
    }

    void Config::SetAddressMapping() {
        // memory addresses are byte addressable, but each request comes with
        // multiple bytes because of bus width, and burst length
        request_size_bytes = bus_width / 8 * BL;
        shift_bits = LogBase2(request_size_bytes);
        int col_low_bits = LogBase2(BL);
        int actual_col_bits = LogBase2(columns) - col_low_bits;

        // has to strictly follow the order of chan, rank, bg, bank, row, col
        std::map<std::string, int> field_widths;
        field_widths["ch"] = LogBase2(channels);
        field_widths["ra"] = LogBase2(ranks);
        field_widths["bg"] = LogBase2(bankgroups);
        field_widths["ba"] = LogBase2(banks_per_group);
        field_widths["ro"] = LogBase2(rows);
        field_widths["co"] = actual_col_bits;

        if (address_mapping.size() != 12) {
            std::cerr << "Unknown address mapping (6 fields each 2 chars required)"
                      << std::endl;
            AbruptExit(__FILE__, __LINE__);
        }

        // // get address mapping position fields from config
        // // each field must be 2 chars
        std::vector<std::string> fields;
        for (size_t i = 0; i < address_mapping.size(); i += 2) {
            std::string token = address_mapping.substr(i, 2);
            fields.push_back(token);
        }

        std::map<std::string, int> field_pos;
        int pos = 0;
        while (!fields.empty()) {
            auto token = fields.back();
            fields.pop_back();
            if (field_widths.find(token) == field_widths.end()) {
                std::cerr << "Unrecognized field: " << token << std::endl;
                AbruptExit(__FILE__, __LINE__);
            }
            field_pos[token] = pos;
            pos += field_widths[token];
        }

        ch_pos = field_pos.at("ch");
        ra_pos = field_pos.at("ra");
        bg_pos = field_pos.at("bg");
        ba_pos = field_pos.at("ba");
        ro_pos = field_pos.at("ro");
        co_pos = field_pos.at("co");

        ch_mask = (1 << field_widths.at("ch")) - 1;
        ra_mask = (1 << field_widths.at("ra")) - 1;
        bg_mask = (1 << field_widths.at("bg")) - 1;
        ba_mask = (1 << field_widths.at("ba")) - 1;
        ro_mask = (1 << field_widths.at("ro")) - 1;
        co_mask = (1 << field_widths.at("co")) - 1;
    }

    void Config::PrintInfo() {
        std::cout << "####TEST_MODE OUTPUT" << std::endl;
        std::cout << "####Configuration Information" << std::endl;
        std::cout << "Channels: " << channels << std::endl;
        std::cout << "Ranks: " << ranks << std::endl;
        std::cout << "Banks: " << banks << std::endl;
        std::cout << "BankGroups: " << bankgroups << std::endl;
        std::cout << "Bank Per Group: " << banks_per_group << std::endl;
        std::cout << "Rows: " << rows << std::endl;
        std::cout << "Columns: " << columns << std::endl;
        std::cout << "Device Width: " << device_width << std::endl;
        std::cout << "Bus Width: " << bus_width << std::endl;
        std::cout << "Devices Per Rank: " << devices_per_rank << std::endl;
        std::cout << "BL: " << BL << std::endl << std::endl;

        std::cout << "####TEST_MODE OUTPUT" << std::endl;
        std::cout << "####Address Mapping Information"
                  << std::endl;
        std::cout << "Address Mapping:      " << address_mapping
                  << std::endl;
        std::cout << "Shift Bits:           " << shift_bits
                  << std::endl;
        std::cout << "Channel Position:     " << std::setw(3) << std::left << ch_pos
                  << "Channel Mask:     " << std::bitset<32>(ch_mask)
                  << std::endl;
        std::cout << "Rank Position:        " << std::setw(3) << std::left << ra_pos
                  << "Rank Mask:        " << std::bitset<32>(ra_mask)
                  << std::endl;
        std::cout << "Bank Groups Position: " << std::setw(3) << std::left << bg_pos
                  << "Bank Groups Mask: " << std::bitset<32>(bg_mask)
                  << std::endl;
        std::cout << "Bank Position:        " << std::setw(3) << std::left << ba_pos
                  << "Bank Mask:        " << std::bitset<32>(ba_mask)
                  << std::endl;
        std::cout << "Row Position:         " << std::setw(3) << std::left << ro_pos
                  << "Row Mask:         " << std::bitset<32>(ro_mask)
                  << std::endl;
        std::cout << "Column Position:      " << std::setw(3) << std::left << co_pos
                  << "Column Mask:      " << std::bitset<32>(co_mask)
                  << std::endl << std::endl;


    }

} //namespace dramfaultsim