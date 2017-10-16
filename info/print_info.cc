#include "TFile.h"
#include "TH1D.h"

#include <vector>

using namespace std;

//----------------------------------------------------------------------------------------------------

int main()
{
	vector<string> datasets = {
		"DS-fill5313",
		"DS-fill5314",
		"DS-fill5317",
		"DS-fill5321",
	};

	vector<string> diagonals = {
		"45b_56t",
		"45t_56b",
	};

	double entries_tot = 0.;

	for (const auto &dataset : datasets)
	{
		for (const auto &diagonal : diagonals)
		{
			printf("\n*%s, %s\n", dataset.c_str(), diagonal.c_str());

			string f_in_name = "../" + dataset + "/distributions_" + diagonal + ".root";
			TFile *f_in = TFile::Open(f_in_name.c_str());

			string h_path = "elastic cuts/cut 1/h_cq1";
			TH1D *h_in = (TH1D *) f_in->Get(h_path.c_str());
			double entries = h_in->GetEntries();

			/*
			string h_path = "acceptance correction/ob-2-10-0.05/h_t_Nev_after_no_corr";
			TH1D *h_in = (TH1D *) f_in->Get(h_path.c_str());

			int bin_l = h_in->FindBin(0.0007);
			int bin_h = h_in->FindBin(0.02);
			//printf("    bin_l = %i\n", bin_l);
			//printf("    bin_h = %i\n", bin_h);

			double entries = 0.;
			for (int bi = bin_l; bi <= bin_h; bi++)
			{
				entries += h_in->GetBinContent(bi);
			}
			*/

			printf("    %.2E\n", entries);

			entries_tot += entries;
		}
	}

	printf("\n*total\n");
	printf("    %.2E\n", entries_tot);

	return 0;
}
