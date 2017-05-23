#include "TFile.h"
#include "TH1D.h"
#include "TRandom2.h"

#include "../common_definitions.h"
#include "../common_algorithms.h"
#include "../AcceptanceCalculator.h"

#include "../DS-fill5313/parameters.h"
#include "../common.h"

#include "scenarios.h"
#include "input_distributions.h"

#include "command_line_tools.h"

#include <vector>
#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: mc_simu [option] [option] ...\n");
	printf("OPTIONS:\n");
	printf("    -diagonal <str>\n");
	printf("    -scenario <str>     systematic scenario\n");
	printf("    -model <str>        dsigma/dt model\n");
	printf("    -events <int>\n");
	printf("    -seed <int>\n");
	printf("    -output <file>\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// defaults
	string diagonalStr = "";
	string scenario = "none";
	string model = "fit2-2";

	unsigned int n_events = (unsigned int) 1e5;
	unsigned int seed = 1;

	string outFileName = "ouput.root";

	double ineff_intercept = 0.067 + 0.03; // sum from 3/4 and 2/4, respectively
	double ineff_slope = 140.;	// rad^-1

	// parse command line
	for (int argi = 1; (argi < argc) && (cl_error == 0); ++argi)
	{
		if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0)
		{
			cl_error = 1;
			continue;
		}

		if (TestStringParameter(argc, argv, argi, "-diagonal", diagonalStr)) continue;
		if (TestStringParameter(argc, argv, argi, "-scenario", scenario)) continue;
		if (TestStringParameter(argc, argv, argi, "-model", model)) continue;

		if (TestUIntParameter(argc, argv, argi, "-events", n_events)) continue;
		if (TestUIntParameter(argc, argv, argi, "-seed", seed)) continue;

		if (TestStringParameter(argc, argv, argi, "-output", outFileName)) continue;

		printf("ERROR: unknown option '%s'.\n", argv[argi]);
		cl_error = 1;
	}

	if (cl_error)
	{
		PrintUsage();
		return 1;
	}

	// print input
	printf(">> input parameters:\n");
	printf("    diagonalStr = %s\n", diagonalStr.c_str());
	printf("    scenario = %s\n", scenario.c_str());
	printf("    model = %s\n", model.c_str());
	printf("    n_events = %u (%.1E)\n", n_events, double(n_events));
	printf("    seed = %u\n", seed);
	printf("    ineff_intercept = %.3E\n", ineff_intercept);
	printf("    ineff_slope = %.3E\n", ineff_slope);
	printf("    outFileName = %s\n", outFileName.c_str());

	// init diagonal settings
	Init(diagonalStr);
	if (diagonal == dCombined || diagonal == ad45b_56b || diagonal == ad45t_56t)
		return rcIncompatibleDiagonal;

	// set scenario
	Environment env_sim = env;
	Environment env_rec = env;
	Analysis anal_sim = anal;
	Analysis anal_rec = anal;

	Biases biases;

	if (SetScenario(scenario, biases, env_sim, anal_sim, env_rec, anal_rec) != 0)
	{
		printf("ERROR: unknown scenario '%s'.\n", scenario.c_str());
		return 2;
	}

	printf(">> biases:\n");
	biases.Print();

	// random seed
	gRandom->SetSeed(seed);

	// initialise acceptance calculation
	AcceptanceCalculator accCalc;
	accCalc.Init(th_y_sign, anal_rec);

	// output file	
	TFile *f_out = new TFile(outFileName.c_str(), "recreate");

	// load input dsigma/dt distribution
	if (LoadTDistributions() != 0)
		return 1;

	TSpline *s_dsdt = NULL;
	TGraph *g_dsdt = NULL;
   	for (const auto &itd : inputTDistributions)
	{
		if (itd.label == model)
		{
			s_dsdt = itd.s_dsdt;
			g_dsdt = itd.g_dsdt;
		}
	}

	if (s_dsdt == NULL)
	{
		printf("ERROR: dsigma/dt model '%s' not available.\n", model.c_str());
		return 5;
	}

	gDirectory = f_out;
	g_dsdt->Write("g_dsdt");

	// list of binnings
	vector<string> binnings;
	binnings.push_back("ob-1-20-0.05");
	binnings.push_back("ob-2-10-0.05");
	binnings.push_back("ob-3-5-0.05");

	// book histograms
	vector<TH1D*> bh_t_tr, bh_t_re;
	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		unsigned int N_bins;
		double *bin_edges;
		BuildBinning(anal, binnings[bi], bin_edges, N_bins);

		bh_t_tr.push_back(new TH1D("", ";|t|;events per bin", N_bins, bin_edges)); bh_t_tr[bi]->Sumw2();
		bh_t_re.push_back(new TH1D("", ";|t|;events per bin", N_bins, bin_edges)); bh_t_re[bi]->Sumw2();
	}

	// simulation settings
	const double be = 6.;
	const double ga = 1. - exp(-be * anal.t_max_full);

	// event loop
	for (unsigned int ev = 0; ev < n_events; ev++)
	{
		// ----- true event -----

		Kinematics k_tr;

		const double u = gRandom->Rndm();
		k_tr.t = - log(1. - ga * u) / be;
		double w = s_dsdt->Eval(k_tr.t) / exp(-be * k_tr.t);

		k_tr.phi = th_y_sign * gRandom->Rndm() * M_PI; // just one hemisphere
	
		k_tr.th = sqrt(k_tr.t) / env_sim.p;
		k_tr.th_x = k_tr.th_x_L = k_tr.th_x_R = k_tr.th * cos(k_tr.phi);
		k_tr.th_y = k_tr.th_y_L = k_tr.th_y_R = k_tr.th * sin(k_tr.phi);
		
		for (unsigned int bi = 0; bi < binnings.size(); ++bi)
			bh_t_tr[bi]->Fill(k_tr.t, w);

		// ----- smearing -----

		double d_x = gRandom->Gaus() * anal_sim.si_th_x_LRdiff;
		double d_y = gRandom->Gaus() * anal_sim.si_th_y_LRdiff;

		double m_x = gRandom->Gaus() * anal_sim.si_th_x_2arm;
		double m_y = gRandom->Gaus() * anal_sim.si_th_y_2arm;

		Kinematics k_sm;

		k_sm.th_x_R = k_tr.th_x + m_x + d_x/2.;
		k_sm.th_x_L = k_tr.th_x + m_x - d_x/2.;

		k_sm.th_y_R = k_tr.th_y + m_y + d_y/2.;
		k_sm.th_y_L = k_tr.th_y + m_y - d_y/2.;

		k_sm.th_x = (k_sm.th_x_L + k_sm.th_x_R) / 2.;
		k_sm.th_y = (k_sm.th_y_L + k_sm.th_y_R) / 2.;
		k_sm.th = sqrt(k_sm.th_x*k_sm.th_x + k_sm.th_y*k_sm.th_y);

		// ----- reconstruction bias -----

		Kinematics k_re;

		k_re.th_x_L = biases.L.sc_th_x * k_sm.th_x_L + biases.L.sh_th_x + biases.L.tilt_th_x_eff_prop_to_th_y * k_sm.th_y_L;
		k_re.th_x_R = biases.R.sc_th_x * k_sm.th_x_R + biases.R.sh_th_x + biases.R.tilt_th_x_eff_prop_to_th_y * k_sm.th_y_R;

		k_re.th_y_L = biases.L.sc_th_y * k_sm.th_y_L + biases.L.sh_th_y + biases.L.tilt_th_y_eff_prop_to_th_x * k_sm.th_x_L;
		k_re.th_y_R = biases.R.sc_th_y * k_sm.th_y_R + biases.R.sh_th_y + biases.R.tilt_th_y_eff_prop_to_th_x * k_sm.th_x_R;

		k_re.th_x = (k_re.th_x_L + k_re.th_x_R) / 2.;
		k_re.th_y = (k_re.th_y_L + k_re.th_y_R) / 2.;
		k_re.th = sqrt(k_re.th_x*k_re.th_x + k_re.th_y*k_re.th_y);
		k_re.t = env_rec.p*env_rec.p * k_re.th*k_re.th;

		// ----- fiducial cuts and acceptance correction -----

		double phi_corr = 0., div_corr = 0.;
		bool skip = accCalc.Calculate(k_re, phi_corr, div_corr);

		// ----- inefficiency and its correction -----
		
		double eff = 1. - (ineff_intercept + ineff_slope * fabs(k_sm.th_y));
		double eff_corr = 1. / (1. - ( (ineff_intercept + biases.eff_intercept) + (ineff_slope + biases.eff_slope) * fabs(k_sm.th_y)));

		double norm_adjustment = eff * eff_corr * (1. + biases.norm);

		// fill plots
		if (!skip)
		{
			for (unsigned int bi = 0; bi < binnings.size(); ++bi)
				bh_t_re[bi]->Fill(k_re.t, w * norm_adjustment * phi_corr * div_corr / 2.);
		}
	}
	
	// save
	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		gDirectory = f_out->mkdir(binnings[bi].c_str());

		bh_t_tr[bi]->Scale(1., "width");
		bh_t_tr[bi]->Write("h_t_tr");

		bh_t_re[bi]->Scale(1., "width");
		bh_t_re[bi]->Write("h_t_re");
	}

	return 0;
}
