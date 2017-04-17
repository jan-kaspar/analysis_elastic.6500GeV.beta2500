#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------

std::vector<std::string> input_files;

void InitInputFiles()
{
	input_files.clear();

	std::string prefix = "root://eostotem.cern.ch//eos/totem/data/offline/2016/2500m/version2/";

	input_files.push_back(prefix + "run_10208.0_ntuple.root");
	input_files.push_back(prefix + "run_10208.1_ntuple.root");
	input_files.push_back(prefix + "run_10208.2_ntuple.root");
	input_files.push_back(prefix + "run_10208.3_ntuple.root");
	input_files.push_back(prefix + "run_10208.4_ntuple.root");
	input_files.push_back(prefix + "run_10209.0_ntuple.root");
	input_files.push_back(prefix + "run_10209.1_ntuple.root");
	input_files.push_back(prefix + "run_10209.2_ntuple.root");
	input_files.push_back(prefix + "run_10210.0_ntuple.root");
	input_files.push_back(prefix + "run_10210.1_ntuple.root");
	input_files.push_back(prefix + "run_10210.2_ntuple.root");
	input_files.push_back(prefix + "run_10212.0_ntuple.root");
	input_files.push_back(prefix + "run_10212.1_ntuple.root");
	input_files.push_back(prefix + "run_10212.2_ntuple.root");
	input_files.push_back(prefix + "run_10212.3_ntuple.root");
	input_files.push_back(prefix + "run_10214.0_ntuple.root");
	input_files.push_back(prefix + "run_10214.1_ntuple.root");
	input_files.push_back(prefix + "run_10214.2_ntuple.root");
	input_files.push_back(prefix + "run_10214.3_ntuple.root");
	input_files.push_back(prefix + "run_10216.0_ntuple.root");
	input_files.push_back(prefix + "run_10216.1_ntuple.root");
	input_files.push_back(prefix + "run_10216.2_ntuple.root");
	input_files.push_back(prefix + "run_10216.3_ntuple.root");
	input_files.push_back(prefix + "run_10218.0_ntuple.root");
	input_files.push_back(prefix + "run_10218.1_ntuple.root");
	input_files.push_back(prefix + "run_10218.2_ntuple.root");
	input_files.push_back(prefix + "run_10218.3_ntuple.root");
	input_files.push_back(prefix + "run_10218.4_ntuple.root");
	input_files.push_back(prefix + "run_10218.5_ntuple.root");
	input_files.push_back(prefix + "run_10220.0_ntuple.root");
	input_files.push_back(prefix + "run_10220.1_ntuple.root");
	input_files.push_back(prefix + "run_10220.2_ntuple.root");
	input_files.push_back(prefix + "run_10220.3_ntuple.root");
	input_files.push_back(prefix + "run_10220.4_ntuple.root");
	input_files.push_back(prefix + "run_10222.0_ntuple.root");
	input_files.push_back(prefix + "run_10222.1_ntuple.root");
	input_files.push_back(prefix + "run_10222.2_ntuple.root");
	input_files.push_back(prefix + "run_10222.3_ntuple.root");
	input_files.push_back(prefix + "run_10222.4_ntuple.root");
	input_files.push_back(prefix + "run_10222.5_ntuple.root");
	input_files.push_back(prefix + "run_10224.0_ntuple.root");
	input_files.push_back(prefix + "run_10224.1_ntuple.root");
	input_files.push_back(prefix + "run_10224.2_ntuple.root");
	input_files.push_back(prefix + "run_10224.3_ntuple.root");
}
