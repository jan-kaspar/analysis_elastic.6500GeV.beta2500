#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------

std::vector<std::string> input_files;

void InitInputFiles()
{
	input_files.clear();

	std::string prefix = "root://eostotem.cern.ch//eos/totem/data/offline/2016/2500m/version1/";

	input_files.push_back(prefix + "run_10190.0_ntuple.root");
	input_files.push_back(prefix + "run_10190.1_ntuple.root");
	input_files.push_back(prefix + "run_10192.0_ntuple.root");
	input_files.push_back(prefix + "run_10192.1_ntuple.root");
	input_files.push_back(prefix + "run_10192.2_ntuple.root");
	input_files.push_back(prefix + "run_10192.3_ntuple.root");
	input_files.push_back(prefix + "run_10194.0_ntuple.root");
	input_files.push_back(prefix + "run_10194.1_ntuple.root");
	input_files.push_back(prefix + "run_10194.2_ntuple.root");
	input_files.push_back(prefix + "run_10194.3_ntuple.root");
	input_files.push_back(prefix + "run_10194.4_ntuple.root");
	input_files.push_back(prefix + "run_10196.0_ntuple.root");
	input_files.push_back(prefix + "run_10196.1_ntuple.root");
	input_files.push_back(prefix + "run_10196.2_ntuple.root");
	input_files.push_back(prefix + "run_10196.3_ntuple.root");
	input_files.push_back(prefix + "run_10196.4_ntuple.root");
	input_files.push_back(prefix + "run_10198.0_ntuple.root");
	input_files.push_back(prefix + "run_10198.1_ntuple.root");
	input_files.push_back(prefix + "run_10198.2_ntuple.root");
	input_files.push_back(prefix + "run_10198.3_ntuple.root");
	input_files.push_back(prefix + "run_10198.4_ntuple.root");
	input_files.push_back(prefix + "run_10200.0_ntuple.root");
	input_files.push_back(prefix + "run_10200.1_ntuple.root");
	input_files.push_back(prefix + "run_10200.2_ntuple.root");
	input_files.push_back(prefix + "run_10200.3_ntuple.root");
	input_files.push_back(prefix + "run_10200.4_ntuple.root");
	input_files.push_back(prefix + "run_10202.0_ntuple.root");
	input_files.push_back(prefix + "run_10202.1_ntuple.root");
	input_files.push_back(prefix + "run_10202.2_ntuple.root");
	input_files.push_back(prefix + "run_10202.3_ntuple.root");
	input_files.push_back(prefix + "run_10204.0_ntuple.root");
	input_files.push_back(prefix + "run_10204.1_ntuple.root");
	input_files.push_back(prefix + "run_10204.2_ntuple.root");
	input_files.push_back(prefix + "run_10204.3_ntuple.root");
	input_files.push_back(prefix + "run_10204.4_ntuple.root");
	input_files.push_back(prefix + "run_10206.0_ntuple.root");
	input_files.push_back(prefix + "run_10206.1_ntuple.root");
}
