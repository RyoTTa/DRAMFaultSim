#include <iostream>
#include "./../ext/headers/args.hxx"
#include "common.h"
#include "configuration.h"
#include "generator.h"

using namespace dramfaultsim;

int main(int argc, const char **argv) {

    args::ArgumentParser parser(
            "DRAM Fault Simulator.",
            "Examples: \n."
            "./build/dramfaultsimmain configs/DDR4_8Gb_x8_3200.ini -c 100 -t "
            "sample_trace.txt\n"
            "./build/dramfaultsimmain configs/DDR4_8Gb_x8_3200.ini -s random -c 100");

    args::HelpFlag help(parser, "help", "Display the help menu", {'h', "help"});
    args::ValueFlag<uint64_t> num_request_arg(parser, "num_request",
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
    args::ValueFlag<std::string> faultmap_file_read_arg(
            parser, "fault map read path",
            "DRAM Fault-Map file, path to read DRAM Fault-Map file",
            {'r', "fmread"});
    args::ValueFlag<std::string> faultmap_file_write_arg(
            parser, "fault map write path",
            "DRAM Fault-Map file, path to write DRAM Fault-Map file",
            {'w', "fmwrite"});
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

    uint64_t request = args::get(num_request_arg);
    std::string output_dir = args::get(output_dir_arg);
    std::string trace_file_path = args::get(trace_file_arg);
    std::string stream_type = args::get(stream_arg);
    std::string faultmap_read_path = args::get(faultmap_file_read_arg);
    std::string faultmap_write_path = args::get(faultmap_file_write_arg);

    Config *config = new Config(config_file, output_dir, request);

    if (!faultmap_read_path.empty()){
#ifdef TEST_MODE
        std::cout << "Fafult Map Read from the path" << std::endl;
#endif
        config->faultmap_read_path = faultmap_read_path;
    }
    if (!faultmap_write_path.empty()){
#ifdef TEST_MODE
        std::cout << "Fafult Map Write to the path" << std::endl;
#endif
        config->faultmap_write_path = faultmap_write_path;
    }

#ifdef TEST_MODE
    config->PrintInfo();
#endif
    Generator *generator;
    if (!trace_file_path.empty()) {
        std::cout << "TraceBasedGenerator" << std::endl;
    } else {
        if (stream_type == "stream" || stream_type == "s") {
            std::cout << "StreamBasedGenerator" << std::endl;
        } else {
            std::cout << "RandomBasedGenerator" << std::endl;
            generator = new RandomGenerator(*config);
        }
    }

    bool last_request = false;
    while (!last_request){
        last_request = generator->AccessMemory();
    }

    //For DRAM Fault Sim Testing Code
#ifdef TEST_MODE
    std::cout << "Main Function End" << std::endl;
#endif  // TEST_MODE


    delete generator;
    delete config;
}