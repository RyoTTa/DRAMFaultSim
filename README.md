# DRAM Fault Sim
DRAM Fault Simulator




DRAMFaultSim Fault Map Load

`./dramfaultsimmain -c ../config/$config_name -o $output_dir_name -r $faultmap_name -p $result_prefix -n $request_number -s $start_round -d $repeat_round`

DRAMFaultSim Fault Map Store

`./dramfaultsimmain -c ../config/$config_name -o $output_dir_name -w $faultmap_name -p $result_prefix -n $request_number -s $start_round -d $repeat_round`

output_dir_name : Fault Result, Stats, Fault Trace output directory name

faultmap_name : Fault Map file to store or load

result_prefix : Prefix name for output result

request_number : The number of memory requests to execute on the memory system

start_round : 


Example)

`./dramfaultsimmain -c ../config/DDR4_4Gb_x16_2666.ini -o ./TEST -w 4Gb_x16 -p TEST_RESULT -n 1 -s 0 -d 1`

`./dramfaultsimmain -c ../config/DDR4_4Gb_x16_2666.ini -o ./TEST -r 4Gb_x16 -p TEST_RESULT -n 10000 -s 0 -d 10`

