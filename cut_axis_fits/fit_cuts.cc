#include "TFile.h"
#include "TProfile.h"
#include "TF1.h"

#include <vector>

using namespace std;

//----------------------------------------------------------------------------------------------------

int main()
{
	// define input
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

	// prepare output
	TFile *f_out = TFile::Open("fit_cuts.root", "recreate");

	// prepare fits
	TF1 *ff = new TF1("ff", "1 ++ x");

	// make fits
	for (const auto &dataset : datasets)
	{
		TDirectory *d_dataset = f_out->mkdir(dataset.c_str());

		for (const auto &diagonal : diagonals)
		{
			printf("\n*%s, %s\n", dataset.c_str(), diagonal.c_str());

			TDirectory *d_diagonal = d_dataset->mkdir(diagonal.c_str());

			string f_in_name = "../" + dataset + "/distributions_" + diagonal + ".root";
			TFile *f_in = TFile::Open(f_in_name.c_str());

			TProfile *p_in;
			double x_min, x_max;

			// ---------- cut 5 ----------

			gDirectory = d_diagonal->mkdir("cut 5");
			printf("    cut 5\n");

			p_in = (TProfile *) f_in->Get("elastic cuts/cut 5/p_cq5");

			x_min = (diagonal == "45b_56t") ? 1.5 : -18.;
			x_max = (diagonal == "45b_56t") ? 18 : -1.5;
			p_in->Fit(ff, "Q", "", x_min, x_max);

			printf("        slope = %.6f +- %.6f\n", ff->GetParameter(1), ff->GetParError(1));

			p_in->Write();

			// ---------- cut 6 ----------

			gDirectory = d_diagonal->mkdir("cut 6");
			printf("    cut 6\n");

			p_in = (TProfile *) f_in->Get("elastic cuts/cut 6/p_cq6");

			x_min = (diagonal == "45b_56t") ? -19 : 1.5;
			x_max = (diagonal == "45b_56t") ? -1.5 : 19;
			p_in->Fit(ff, "Q", "", x_min, x_max);

			printf("        slope = %.6f +- %.6f\n", ff->GetParameter(1), ff->GetParError(1));

			p_in->Write();

			// ---------- cut 7 ----------

			gDirectory = d_diagonal->mkdir("cut 7");
			printf("    cut 7\n");

			p_in = (TProfile *) f_in->Get("elastic cuts/cut 7/p_cq7");

			x_min = -60E-6;
			x_max = +60E-6;
			p_in->Fit(ff, "Q", "", x_min, x_max);

			printf("        slope = %.0f +- %.0f\n", ff->GetParameter(1), ff->GetParError(1));

			p_in->Write();
		}
	}

	// clean up
	delete f_out;

	return 0;
}
