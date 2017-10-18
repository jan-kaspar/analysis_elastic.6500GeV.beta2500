#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"

#include <string>
#include <vector>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

TF1 *ff = NULL;

void ProcessOne(TH1D *h)
{
	// settings
	double t_fit_min = 0.01, t_fit_max = 0.05;
	double t_sum_min = 0.01, t_sum_max = 0.5;

	double si_el_ref = 31.5;	// mb

	// determine limits
	int bi_fit_min = h->GetXaxis()->FindBin(t_fit_min);
	int bi_sum_min = h->GetXaxis()->FindBin(t_sum_min);
	int bi_sum_max = h->GetXaxis()->FindBin(t_sum_max);
	double t_sum_integrate_border = h->GetXaxis()->GetBinLowEdge(bi_sum_min);

	// fit
	printf("        fit from %.4f to %.4f\n", t_fit_min, t_fit_max);

	ff->SetParameters(h->GetBinContent(bi_fit_min), 20.);
	h->Fit(ff, "Q", "", t_fit_min, t_fit_max);
	h->Write();

	// calculate integral components
	printf("        integrating fit from 0 to %.4f\n", t_sum_integrate_border);
	double c_extr = ff->Integral(0., t_sum_integrate_border);

	printf("        summing from bin %i (left edge %.4f) to bin %i (right edge %.4f)\n",
		bi_sum_min, h->GetBinLowEdge(bi_sum_min),
		bi_sum_max, h->GetBinLowEdge(bi_sum_max) + h->GetBinWidth(bi_sum_max));

	double c_hist = 0;
	for (int bi = bi_fit_min; bi <= bi_sum_max; ++bi)
	{
		c_hist += h->GetBinContent(bi) * h->GetBinWidth(bi);
	}

	double c_full = c_extr + c_hist;

	// print results
	printf("        c_extr = %.4E, c_hist = %.4E, c_full = %.4E\n", c_extr, c_hist, c_full);

	printf("            L = %.4E\n", c_full / si_el_ref);
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// configuration
	string binning = "ob-2-10-0.05";

	vector<string> datasets;
	datasets.push_back("DS-fill5313");
	datasets.push_back("DS-fill5314");
	datasets.push_back("DS-fill5317");
	datasets.push_back("DS-fill5321");

	datasets.push_back("DS-fill5313-nom-opt");
	datasets.push_back("DS-fill5314-nom-opt");
	datasets.push_back("DS-fill5317-nom-opt");
	datasets.push_back("DS-fill5321-nom-opt");

	datasets.push_back("DS-firstParts");
	datasets.push_back("DS-lastParts");

	datasets.push_back("DS-simulation");

	vector<string> diagonals;
	diagonals.push_back("45b_56t");
	diagonals.push_back("45t_56b");
	//diagonals.push_back("combined");

	// initializations
	ff = new TF1("ff", "[0] * exp(-[1]*x)");

	// prepare output (validation)
	TFile *f_out = TFile::Open("normalization.root", "recreate");

	// process data
	for (unsigned int dsi = 0; dsi < datasets.size(); dsi++)
	{
		printf("\n\n%s\n", datasets[dsi].c_str());

		TDirectory *d_dataset = f_out->mkdir(datasets[dsi].c_str());

		for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
		{
			printf("    %s\n", diagonals[dgni].c_str());

			TFile *f_in = TFile::Open(("../"+datasets[dsi]+"/distributions_"+diagonals[dgni]+".root").c_str());
			TH1D *h_in = (TH1D *) f_in->Get(("normalization/"+binning+"/h_t_normalized_no_L").c_str());

			gDirectory = d_dataset->mkdir(diagonals[dgni].c_str());

			ProcessOne(h_in);

			delete f_in;
		}
	}

	// clean up
	delete f_out;

	return 0;
}
