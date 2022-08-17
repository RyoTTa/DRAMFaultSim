#include <iostream>
#include "./../ext/headers/args.hxx"
#include "common.h"
#include "configuration.h"

using namespace dramfaultsim;

int main(int argc, const char **argv) {

    args::ArgumentParser parser(
            "DRAM Fault Simulator.",
            "Examples: \n."
            "./build/dramfaultsimmain configs/DDR4_8Gb_x8_3200.ini -c 100 -t "
            "sample_trace.txt\n"
            "./build/dramfaultsimmain configs/DDR4_8Gb_x8_3200.ini -s random -c 100");

    args::HelpFlag help(parser, "help", "Display the help menu", {'h', "help"});
    args::ValueFlag<uint64_t> num_cycles_arg(parser, "num_request",
                                             "Number of request to simulate",
                                             {'n', "num-request"}, 10);
    args::ValueFlag<std::string> output_dir_arg(
            parser, "output_dir", "Output directory for stats files",
            {'o', "output-dir"}, ".");
    args::ValueFlag<std::string> stream_arg(
            parser, "stream_type", "address stream generator - (random), stream",
            {'s', "stream"}, "random");
    args::ValueFlag<std::string> trace_file_arg(
            parser, "trace",
            "Trace file, setting this option will ignore -s option",
            {'t', "trace"});
    args::ValueFlag<std::string> config_arg(
            parser, "config", "The config file name (mandatory)",
            {'c', "config"});


    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    std::string config_file = args::get(config_arg);
    if (config_file.empty()) {
        std::cerr << parser;
        return 1;
    }

    uint64_t cycles = args::get(num_cycles_arg);
    std::string output_dir = args::get(output_dir_arg);
    std::string trace_file = args::get(trace_file_arg);
    std::string stream_type = args::get(stream_arg);

    if (!trace_file.empty()) {
        std::cout << "TraceBasedRun" << std::endl;
    } else {
        if (stream_type == "stream" || stream_type == "s") {
            std::cout << "StreamBasedRun" << std::endl;
        } else {
            std::cout << "RandomBasedRun" << std::endl;
        }
    }

    //For DRAM Fault Sim Testing Code
#ifdef TEST_MODE
    Config *config = new Config(config_file, output_dir);
    config->PrintInfo();

    Address addr = config->AddressMapping(0x151213153);

    PrintAddress(addr);

    std::cout << "Main Function End" << std::endl;
#endif  // TEST_MODE

}