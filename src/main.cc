#include <iostream>
#include "./../ext/headers/args.hxx"
#include "common.h"
#include "configuration.h"
#include "generator.h"
#include "stat.h"

using namespace dramfaultsim;

int main(int argc, const char **argv) {

    args::ArgumentParser parser(
            "DRAM Fault Simulator.",
            "Examples: \n."
            "./build/dramfaultsimmain configs/DDR4_8Gb_x8_3200.ini -r faultmap.bin -n 100 -t sample.txt\n"
            "./build/dramfaultsimmain configs/DDR4_8Gb_x8_3200.ini -n 100");

    args::HelpFlag help(parser, "help", "Display the help menu", {'h', "help"});
    args::ValueFlag<uint64_t> num_request_arg(parser, "num_request",
                                              "Number of request to simulate",
                                              {'n', "num-request"}, 10);
    args::ValueFlag<std::string> output_dir_arg(
            parser, "output_dir", "Output directory for stats files",
            {'o', "output-dir"}, ".");
    args::ValueFlag<std::string> output_prefix_arg(
            parser, "output_prefix", "Output prefix for stats and fault trace file",
            {'p', "output-prefix"}, "current");
    args::ValueFlag<std::string> trace_file_arg(
            parser, "trace",
            "Trace file, setting this option will ignore generator configuration",
            {'t', "trace"});
    args::ValueFlag<int> repeat_round_arg(
            parser, "repeat",
            "Repeat Round, Number of repetitions of the experiment",
            {'d', "repeat_round"}, 1);
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
    std::string output_prefix = args::get(output_prefix_arg);
    std::string trace_file_path = args::get(trace_file_arg);
    int repeat_round = args::get(repeat_round_arg);
    std::string faultmap_read_path = args::get(faultmap_file_read_arg);
    std::string faultmap_write_path = args::get(faultmap_file_write_arg);

    Config *config = new Config(config_file, output_dir, output_prefix, request, faultmap_read_path,
                                faultmap_write_path);
    Stat *stat = new Stat(*config);


#ifdef TEST_MODE
    config->PrintInfo();
#endif
    Generator *generator;
    if (!trace_file_path.empty()) {
        std::cout << "TraceBasedGenerator" << std::endl;
    } else {
        if (config->generator_system == "SequentialGenerator") {
            std::cout << "SequentialBasedGenerator" << std::endl;
            generator = new SequentialGenerator(*config, *stat);
        } else {
            std::cout << "RandomBasedGenerator" << std::endl;
            generator = new RandomGenerator(*config, *stat);
        }
    }

    for (int i = 0; i < repeat_round; i++) {
        stat->ResetStat();

        config->repeat_round = i;
        generator->memory_system_->ResetFaultResult();

        std::cout << "Test Round: " << i << std::endl;

        bool last_request = false;
        while (!last_request) {
            last_request = generator->AccessMemory();
        }

        stat->PrintStat();
    }


    //For DRAM Fault Sim Testing Code
#ifdef TEST_MODE
    std::cout << "Main Function End" << std::endl;
#endif  // TEST_MODE

    delete generator;
    delete stat;
    delete config;

}