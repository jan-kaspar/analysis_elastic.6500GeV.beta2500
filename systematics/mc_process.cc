#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"

#include <vector>
#include <string>

#include "command_line_tools.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

TH1D* GetHistogram(const string &dir, const string &model, const string &scenario, const string &binning, const string &obj)
{
	string file = dir + "/" + model + "/" + scenario + ".root";
	TFile *f_in = TFile::Open(file.c_str());
	if (!f_in)
	{
		printf("ERROR: can't open file '%s'.\n", file.c_str());
		return NULL;
	}

	string path = binning + "/" + obj;
	TH1D *h_orig = (TH1D *) f_in->Get(path.c_str());
	if (!h_orig)
	{
		printf("ERROR: can't load object '%s'.\n", path.c_str());
		return NULL;
	}

	gDirectory = NULL;	// not to associate h_ret with f_in which will be closed in a moment
	TH1D *h_ret = new TH1D(*h_orig);

	delete f_in;

	return h_ret;
}

//----------------------------------------------------------------------------------------------------

/// To be kept synchronised with function "ProcessOne" in "../normalisation/normalisation.cc"
double GetNormalisation(TH1D *h)
{
	// settings
	double t_fit_min = 0.01, t_fit_max = 0.05;
	double t_sum_min = 0.01, t_sum_max = 0.5;

	double si_el_ref = 29.7;	// mb

	// determine limits
	int bi_fit_min = h->GetXaxis()->FindBin(t_fit_min);
	int bi_sum_min = h->GetXaxis()->FindBin(t_sum_min);
	int bi_sum_max = h->GetXaxis()->FindBin(t_sum_max);
	double t_sum_integrate_border = h->GetXaxis()->GetBinLowEdge(bi_sum_min);

	// fit
	//printf("        fit from %.4f to %.4f\n", t_fit_min, t_fit_max);

	TF1 *ff = new TF1("ff", "[0] * exp(-[1]*x)");

	ff->SetParameters(h->GetBinContent(bi_fit_min), 20.);
	h->Fit(ff, "Q", "", t_fit_min, t_fit_max);

	// calculate integral components
	//printf("        integrating fit from 0 to %.4f\n", t_sum_integrate_border);
	double c_extr = ff->Integral(0., t_sum_integrate_border);

	delete ff;

	/*
	printf("        summing from bin %i (left edge %.4f) to bin %i (right edge %.4f)\n",
		bi_sum_min, h->GetBinLowEdge(bi_sum_min),
		bi_sum_max, h->GetBinLowEdge(bi_sum_max) + h->GetBinWidth(bi_sum_max));
	*/

	double c_hist = 0;
	for (int bi = bi_fit_min; bi <= bi_sum_max; ++bi)
	{
		c_hist += h->GetBinContent(bi) * h->GetBinWidth(bi);
	}

	double c_full = c_extr + c_hist;

	return si_el_ref / c_full;
}

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: mc_process [option] [option] ...\n");
	printf("OPTIONS:\n");
	printf("    -input <directory>\n");
	printf("    -output <file>\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// defaults
	string inputDirectory = "simu";

	string outFileName = "mc_process.root";

	string ref_scenario = "none";

	string model_base = "fit2-2";
	string model_secondary = "fit2-4";

	struct Scenario
	{
		string label;
		enum Mode { mDsdt, mUnsmearing } mode;
	};

	vector<Scenario> scenarios = {
		{ "none", Scenario::mDsdt },
		{ "sh-thx", Scenario::mDsdt },
		{ "sh-thx-LRasym", Scenario::mDsdt },
		{ "sh-thy", Scenario::mDsdt },
		{ "sh-thy-TBuncor", Scenario::mDsdt },
		{ "sh-thy-LRasym", Scenario::mDsdt },
		{ "tilt-thx-thy", Scenario::mDsdt },
		{ "tilt-thx-thy-LRasym", Scenario::mDsdt },
		{ "sc-thx", Scenario::mDsdt },
		{ "sc-thx-LRasym", Scenario::mDsdt },
		{ "sc-thy", Scenario::mDsdt },
		{ "sc-thy-LRasym", Scenario::mDsdt },
		{ "sc-thxy-mode1", Scenario::mDsdt },
		{ "sc-thxy-mode2", Scenario::mDsdt },
		{ "sc-thxy-mode3", Scenario::mDsdt },
		{ "dx-sigma", Scenario::mDsdt },
		{ "dy-sigma", Scenario::mDsdt },
		{ "dx-non-gauss", Scenario::mDsdt },
		{ "mx-sigma", Scenario::mUnsmearing },
		{ "my-sigma", Scenario::mUnsmearing },
		{ "eff-intercept", Scenario::mDsdt },
		{ "eff-slope", Scenario::mDsdt },
		{ "beam-mom", Scenario::mDsdt },
		{ "norm", Scenario::mDsdt },
	};

	// parse command line
	for (int argi = 1; (argi < argc) && (cl_error == 0); ++argi)
	{
		if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0)
		{
			cl_error = 1;
			continue;
		}

		if (TestStringParameter(argc, argv, argi, "-input", inputDirectory)) continue;

		if (TestStringParameter(argc, argv, argi, "-output", outFileName)) continue;

		printf("ERROR: unknown option '%s'.\n", argv[argi]);
		cl_error = 1;
	}

	if (cl_error)
	{
		PrintUsage();
		return 1;
	}

	// list of binnings
	vector<string> binnings;
	binnings.push_back("ob-1-20-0.05");
	binnings.push_back("ob-2-10-0.05");
	binnings.push_back("ob-3-5-0.05");

	// output file	
	TFile *f_out = new TFile(outFileName.c_str(), "recreate");

	// process all scenarios
	for (const auto &scenario : scenarios)
	{
		TDirectory *d_scenario = f_out->mkdir(scenario.label.c_str());

		for (const string &binning : binnings)
		{
			TDirectory *d_binning = d_scenario->mkdir(binning.c_str());

			// get reference histograms
			TH1D *h_t_tr_ref = GetHistogram(inputDirectory, model_base, ref_scenario, binning, "h_t_tr");
			TH1D *h_t_re_ref = GetHistogram(inputDirectory, model_base, ref_scenario, binning, "h_t_re");

			// get histogram with effect
			TH1D *h_t_re = GetHistogram(inputDirectory, model_base, scenario.label, binning, "h_t_re");

			// validate input
			if (!h_t_tr_ref || !h_t_re_ref || !h_t_re)
				continue;

			// make corrected histograms for scaling
			TH1D *h_t_1 = NULL;
			TH1D *h_t_0 = NULL;
			if (scenario.mode == Scenario::mDsdt)
			{
				TH1D *h_unsm_corr = new TH1D(*h_t_tr_ref);
				h_unsm_corr->Divide(h_t_re_ref);

				h_t_1 = new TH1D(*h_t_re);
				h_t_1->Multiply(h_unsm_corr);

				h_t_0 = new TH1D(*h_t_tr_ref);

				delete h_unsm_corr;
			}

			if (scenario.mode == Scenario::mUnsmearing)
			{
				TH1D *h_unsm_corr = new TH1D(*h_t_tr_ref);
				h_unsm_corr->Divide(h_t_re);

				h_t_1 = new TH1D(*h_t_re_ref);
				h_t_1->Multiply(h_unsm_corr);

				h_t_0 = new TH1D(*h_t_re_ref);

				delete h_unsm_corr;
			}

			// normalise histograms
			double n_0 = GetNormalisation(h_t_0);
			h_t_0->Scale(n_0);

			double n_1 = GetNormalisation(h_t_1);
			h_t_1->Scale(n_1);

			// evaluate effect
			TH1D *h_eff = new TH1D(*h_t_1);
			h_eff->Divide(h_t_0);

			gDirectory = d_binning;
			h_eff->Write("h_eff");

			// clean up
			delete h_t_tr_ref;
			delete h_t_re_ref;
			delete h_t_re;
			delete h_t_1;
			delete h_t_0;
			delete h_eff;
		}
	}

	// estimate uncertainty due to smearing model uncertainty
	TDirectory *d_scenario = f_out->mkdir("unsmearing-model");

	for (const string &binning : binnings)
	{
		TDirectory *d_binning = d_scenario->mkdir(binning.c_str());

		// get histograms
		TH1D *h_t_tr_base = GetHistogram(inputDirectory, model_base, ref_scenario, binning, "h_t_tr");
		TH1D *h_t_re_base = GetHistogram(inputDirectory, model_base, ref_scenario, binning, "h_t_re");

		TH1D *h_t_tr_secondary = GetHistogram(inputDirectory, model_secondary, ref_scenario, binning, "h_t_tr");
		TH1D *h_t_re_secondary = GetHistogram(inputDirectory, model_secondary, ref_scenario, binning, "h_t_re");

		// validate input
		if (!h_t_tr_base || !h_t_re_base || !h_t_tr_secondary || !h_t_re_secondary)
			continue;

		// calculate unsmearing correction (secondary model)
		TH1D *h_unsm_corr = new TH1D(*h_t_tr_secondary);
		h_unsm_corr->Divide(h_t_re_secondary);

		// apply unsmearing correction to base reco histogram
		TH1D *h_t_bias = new TH1D(*h_t_re_base);
		h_t_bias->Multiply(h_unsm_corr);

		// normalise histograms
		double n_base = GetNormalisation(h_t_tr_base);
		h_t_tr_base->Scale(n_base);

		double n_bias = GetNormalisation(h_t_bias);
		h_t_bias->Scale(n_bias);

		// evaluate effect
		TH1D *h_eff = new TH1D(*h_t_bias);
		h_eff->Divide(h_t_tr_base);

		gDirectory = d_binning;
		h_eff->Write("h_eff");

		// clean up
		delete h_unsm_corr;
		delete h_t_bias;
		delete h_eff;
	}

	return 0;
}
