#include "TFile.h"
#include "TGraph.h"
#include "TF1.h"

#include <vector>
#include <string>

#include "command_line_tools.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

TGraph* GetGraph(const string &dir, const string &model, const string &scenario, const string &obj)
{
	string file = dir + "/" + model + "/" + scenario + ".root";
	TFile *f_in = TFile::Open(file.c_str());
	if (!f_in)
	{
		printf("ERROR: can't open file '%s'.\n", file.c_str());
		return NULL;
	}

	string path = obj;
	TGraph *h_orig = (TGraph *) f_in->Get(path.c_str());
	if (!h_orig)
	{
		printf("ERROR: can't load object '%s'.\n", path.c_str());
		return NULL;
	}

	gDirectory = NULL;	// not to associate h_ret with f_in which will be closed in a moment
	TGraph *h_ret = new TGraph(*h_orig);

	delete f_in;

	return h_ret;
}

//----------------------------------------------------------------------------------------------------

TGraph* Divide(const TGraph *g_num, const TGraph *g_den)
{
	TGraph *g_out = new TGraph(*g_num);

	for (int i = 0; i < g_out->GetN(); i++)
	{
		double x, y;
		g_out->GetPoint(i, x, y);

		const double d = g_den->Eval(x);
		const double r = (d == 0.) ? 0. : y / d;

		g_out->SetPoint(i, x, r);
	}

	return g_out;
}

//----------------------------------------------------------------------------------------------------

TGraph* Multiply(const TGraph *g1, const TGraph *g2)
{
	TGraph *g_out = new TGraph(*g1);

	for (int i = 0; i < g_out->GetN(); i++)
	{
		double x, y;
		g_out->GetPoint(i, x, y);

		const double m = g2->Eval(x);

		g_out->SetPoint(i, x, y * m);
	}

	return g_out;
}

//----------------------------------------------------------------------------------------------------

void Scale(TGraph *g, double s)
{
	for (int i = 0; i < g->GetN(); i++)
	{
		double x, y;
		g->GetPoint(i, x, y);
		g->SetPoint(i, x, y * s);
	}
}

//----------------------------------------------------------------------------------------------------

/// To be kept synchronised with function "ProcessOne" in "../normalisation/normalisation.cc"
double GetNormalisation(TGraph *g)
{
	// settings
	double t_fit_min = 0.01, t_fit_max = 0.05;
	double t_sum_min = 0.01, t_sum_max = 0.5;

	double si_el_ref = 29.7;	// mb

	// fit
	TF1 *ff = new TF1("ff", "[0] * exp(-[1]*x)");

	ff->SetParameters(g->Eval(t_fit_min), 20.);
	g->Fit(ff, "Q", "", t_fit_min, t_fit_max);

	// integrate fit (extrapolation)
	double c_extr = ff->Integral(0., t_sum_min);

	delete ff;

	// integrate graph
	double c_graph = 0.;
	const unsigned int n_div = 1000;
	double w = (t_sum_max - t_sum_min) / n_div;
	double v_left = g->Eval(t_sum_min);
	for (unsigned int i = 0; i < n_div; i++)
	{
		double t_right = t_sum_min + double(i + 1) * w;
		double v_right = g->Eval(t_right);

		c_graph += (v_left + v_right) / 2. * w;

		v_left = v_right;
	}

	// evaluate normalisation factor
	double c_full = c_extr + c_graph;

	return si_el_ref / c_full;

	return 1.;
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
	string inputDirectory = ".";

	string outFileName = "ni_process.root";

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
		{ "sc-thx", Scenario::mDsdt },
		{ "sc-thy", Scenario::mDsdt },
		{ "dx-sigma", Scenario::mDsdt },
		{ "dy-sigma", Scenario::mDsdt },
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

	// output file	
	TFile *f_out = new TFile(outFileName.c_str(), "recreate");

	// process all scenarios
	for (const auto &scenario : scenarios)
	{
		//printf("* %s\n", scenario.label.c_str());

		TDirectory *d_scenario = f_out->mkdir(scenario.label.c_str());

		// get reference histograms
		TGraph *g_tr_ref = GetGraph(inputDirectory, model_base, ref_scenario, "g_dsdt_true");
		TGraph *g_re_ref = GetGraph(inputDirectory, model_base, ref_scenario, "g_dsdt_reco");

		// get histogram with effect
		TGraph *g_re = GetGraph(inputDirectory, model_base, scenario.label, "g_dsdt_reco");

		// validate input
		if (!g_tr_ref || !g_re_ref || !g_re)
			continue;

		// make corrected histograms for scaling
		TGraph *g_1 = NULL;
		TGraph *g_0 = NULL;
		if (scenario.mode == Scenario::mDsdt)
		{
			TGraph *g_unsm_corr = Divide(g_tr_ref, g_re_ref);
			g_1 = Multiply(g_re, g_unsm_corr);

			g_0 = new TGraph(*g_tr_ref);

			delete g_unsm_corr;
		}

		if (scenario.mode == Scenario::mUnsmearing)
		{
			TGraph *g_unsm_corr = Divide(g_tr_ref, g_re);
			g_1 = Multiply(g_re_ref, g_unsm_corr);

			g_0 = new TGraph(*g_re_ref);

			delete g_unsm_corr;
		}

		// normalise histograms
		double n_0 = GetNormalisation(g_0);
		Scale(g_0, n_0);

		double n_1 = GetNormalisation(g_1);
		Scale(g_1, n_1);

		// evaluate effect
		TGraph *g_eff = Divide(g_1, g_0);

		gDirectory = d_scenario;
		g_eff->Write("g_eff");

		// clean up
		delete g_0;
		delete g_1;
		delete g_eff;
	}

	// model uncertainty of the unsmearing correction
	TDirectory *d_scenario = f_out->mkdir("unsmearing-model");

	{
		// get histograms
		TGraph *g_tr_base = GetGraph(inputDirectory, model_base, ref_scenario, "g_dsdt_true");
		TGraph *g_re_base = GetGraph(inputDirectory, model_base, ref_scenario, "g_dsdt_reco");

		TGraph *g_tr_secondary = GetGraph(inputDirectory, model_secondary, ref_scenario, "g_dsdt_true");
		TGraph *g_re_secondary = GetGraph(inputDirectory, model_secondary, ref_scenario, "g_dsdt_reco");

		if (g_tr_base && g_re_base && g_tr_secondary && g_re_secondary)
		{
			// calculate unsmearing correction (secondary model)
			TGraph *g_unsm_corr = Divide(g_tr_secondary, g_re_secondary);

			// apply unsmearing correction to base reco histogram
			TGraph *g_bias = Multiply(g_re_base, g_unsm_corr);

			// normalise histograms
			double n_base = GetNormalisation(g_tr_base);
			Scale(g_tr_base, n_base);

			double n_bias = GetNormalisation(g_bias);
			Scale(g_bias, n_bias);

			// evaluate effect
			TGraph *g_eff = Divide(g_bias, g_tr_base);

			gDirectory = d_scenario;
			g_eff->Write("g_eff");

			// clean up
			delete g_unsm_corr;
			delete g_bias;
			delete g_eff;
		}
	}

	return 0;
}
