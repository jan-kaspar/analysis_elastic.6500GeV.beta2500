#include <string>
#include <vector>

//----------------------------------------------------------------------------------------------------

std::vector<std::string> input_files;

void InitInputFiles()
{
	input_files.clear();

	std::string prefix = "root://eostotem.cern.ch//eos/totem/data/offline/2016/2500m/version1/";

	input_files.push_back(prefix + "run_10281.0_ntuple.root");
	input_files.push_back(prefix + "run_10281.1_ntuple.root");
	input_files.push_back(prefix + "run_10283.0_ntuple.root");
	input_files.push_back(prefix + "run_10283.1_ntuple.root");
	input_files.push_back(prefix + "run_10285.0_ntuple.root");
	input_files.push_back(prefix + "run_10285.1_ntuple.root");
	input_files.push_back(prefix + "run_10286.0_ntuple.root");
	input_files.push_back(prefix + "run_10288.0_ntuple.root");
	input_files.push_back(prefix + "run_10289.0_ntuple.root");
	input_files.push_back(prefix + "run_10292.0_ntuple.root");
	input_files.push_back(prefix + "run_10293.0_ntuple.root");
	input_files.push_back(prefix + "run_10294.0_ntuple.root");
	input_files.push_back(prefix + "run_10294.1_ntuple.root");
	input_files.push_back(prefix + "run_10294.2_ntuple.root");
	input_files.push_back(prefix + "run_10294.3_ntuple.root");
	input_files.push_back(prefix + "run_10294.4_ntuple.root");
	input_files.push_back(prefix + "run_10294.5_ntuple.root");
	input_files.push_back(prefix + "run_10294.6_ntuple.root");
	input_files.push_back(prefix + "run_10295.0_ntuple.root");
	input_files.push_back(prefix + "run_10295.1_ntuple.root");
	input_files.push_back(prefix + "run_10295.2_ntuple.root");
	input_files.push_back(prefix + "run_10295.3_ntuple.root");
	input_files.push_back(prefix + "run_10295.4_ntuple.root");
	input_files.push_back(prefix + "run_10295.5_ntuple.root");
	input_files.push_back(prefix + "run_10295.6_ntuple.root");
	input_files.push_back(prefix + "run_10295.7_ntuple.root");
	input_files.push_back(prefix + "run_10295.8_ntuple.root");
	input_files.push_back(prefix + "run_10295.9_ntuple.root");
	input_files.push_back(prefix + "run_10297.0_ntuple.root");
	input_files.push_back(prefix + "run_10297.1_ntuple.root");
	input_files.push_back(prefix + "run_10297.2_ntuple.root");
	input_files.push_back(prefix + "run_10297.3_ntuple.root");
	input_files.push_back(prefix + "run_10298.0_ntuple.root");
	input_files.push_back(prefix + "run_10298.1_ntuple.root");
	input_files.push_back(prefix + "run_10298.2_ntuple.root");
	input_files.push_back(prefix + "run_10299.0_ntuple.root");
	input_files.push_back(prefix + "run_10301.0_ntuple.root");
	input_files.push_back(prefix + "run_10301.1_ntuple.root");
	input_files.push_back(prefix + "run_10302.0_ntuple.root");
	input_files.push_back(prefix + "run_10303.0_ntuple.root");
	input_files.push_back(prefix + "run_10303.1_ntuple.root");
	input_files.push_back(prefix + "run_10303.2_ntuple.root");
	input_files.push_back(prefix + "run_10303.3_ntuple.root");
	input_files.push_back(prefix + "run_10305.0_ntuple.root");
	input_files.push_back(prefix + "run_10305.1_ntuple.root");
	input_files.push_back(prefix + "run_10305.2_ntuple.root");
	input_files.push_back(prefix + "run_10305.3_ntuple.root");
}
