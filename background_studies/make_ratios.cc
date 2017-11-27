#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"

#include <string>
#include <vector>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

int main()
{
	// configuration
	string binning = "ob-3-5-0.05";

	vector<string> datasets;
	datasets.push_back("DS-fill5313");
	datasets.push_back("DS-fill5314");
	datasets.push_back("DS-fill5317");
	datasets.push_back("DS-fill5321");

	vector<string> diagonals;
	diagonals.push_back("45b_56t");
	diagonals.push_back("45t_56b");

	vector<string> anti_diagonals;
	anti_diagonals.push_back("anti_45b_56b");
	anti_diagonals.push_back("anti_45t_56t");

	// process data
	for (unsigned int dsi = 0; dsi < datasets.size(); dsi++)
	{
		printf("* %s\n", datasets[dsi].c_str());
	
		TFile *f_out = TFile::Open((datasets[dsi] + "/ratios.root").c_str(), "recreate");
	
		string dir = datasets[dsi] + "/cuts:1,2";

		for (const string &dgn : diagonals)
		{
			for (const string &adgn : anti_diagonals)
			{
				TFile *f_in_d = TFile::Open((dir + "/distributions_"+dgn+".root").c_str());
				TH1D *h_in_d = (TH1D *) f_in_d->Get(("acceptance correction/"+binning+"/h_t_after").c_str());

				TFile *f_in_ad = TFile::Open((dir + "/distributions_"+adgn+".root").c_str());
				TH1D *h_in_ad = (TH1D *) f_in_ad->Get(("acceptance correction/"+binning+"/h_t_after").c_str());

				h_in_ad->Divide(h_in_d);

				gDirectory = f_out;
				h_in_ad->Write((adgn + "_over_" + dgn).c_str());
			}
		}

		delete f_out;
	}

	return 0;
}
