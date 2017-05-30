#include "TRandom2.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TSpline.h"

#include "common_definitions.h"
#include "common_algorithms.h"
#include "AcceptanceCalculator.h"
#include "stat.h"

#include "parameters_mc.h"

//----------------------------------------------------------------------------------------------------

TSpline* BuildSpline(TGraph *g)
{
	TSpline3 *s = new TSpline3("", g->GetX(), g->GetY(), g->GetN());
	s->SetName(g->GetName());
	return s;
}

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: mc_resolution_details [options]\n");
	printf("OPTIONS:\n");
	printf("    -debug         run in verbose mode\n");
	printf("    -seed <int>    set seed of random generator\n");
	printf("    -events <int>  set number of events\n");
	printf("    -t-max <val>   set maximum value of |t|\n");
	printf("    -alg-old       use old acceptance calculation algorithm\n");
	printf("    -alg-new       use old acceptance calculation algorithm\n");
	printf("    -cuts-flat     use flat (= theta*_x independent) fiducial cuts\n");
	printf("    -cuts-curved   use realistic fiducial cuts (only compatible with -alg-new)\n");
	printf("    -output <file> set output file name\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	// defaults
	unsigned int seed = 1;
	unsigned long N_ev = 1E6;

	bool debug = false;

	bool cuts_flat = false;

	bool acceptance_algorithm_new = true;

	double t_max_full = 1.1;

	string outputFileName = "mc_resolution_details.root";

	// parse command line
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-debug") == 0)
		{
			debug = true;
			continue;
		}

		if (strcmp(argv[i], "-alg-old") == 0)
		{
			acceptance_algorithm_new = false;
			continue;
		}

		if (strcmp(argv[i], "-alg-new") == 0)
		{
			acceptance_algorithm_new = true;
			continue;
		}

		if (strcmp(argv[i], "-cuts-flat") == 0)
		{
			cuts_flat = true;
			continue;
		}

		if (strcmp(argv[i], "-cuts-curved") == 0)
		{
			cuts_flat = false;
			continue;
		}

		if (strcmp(argv[i], "-seed") == 0)
		{
			if (argc-1 > i)
				seed = atoi(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-events") == 0)
		{
			if (argc-1 > i)
				N_ev = (unsigned long) atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-t-max") == 0)
		{
			if (argc-1 > i)
				t_max_full = atof(argv[++i]);
			continue;
		}

		if (strcmp(argv[i], "-output") == 0)
		{
			if (argc-1 > i)
				outputFileName = argv[++i];
			continue;
		}

		printf("ERROR: unknown parameter `%s'.\n", argv[i]);
		PrintUsage();
		return 3;
	}

	// print settings
	printf(">> mc executed with these parameters:\n");
	printf("\tdebug = %u\n", debug);
	printf("\tseed = %u\n", seed);
	printf("\tevents = %lu (%.1E)\n", N_ev, double(N_ev));
	printf("\tacceptance_algorithm_new = %u\n", acceptance_algorithm_new);
	printf("\tcuts_flat = %u\n", cuts_flat);
	printf("\tt_max_full = %.3f\n", t_max_full);
	printf("\toutputFileName = %s\n", outputFileName.c_str());

	// simulation seed
	gRandom->SetSeed(seed);

	// load settings from parameters.h
	Init_base();

	th_y_sign = +1.;
	Init_45b_56t();

	//th_y_sign = -1.;
	//Init_45t_56b();

	// initialise environments
	Environment env_nom = env;		// nominal values from parameters.h

	// initialise analysis objects
	anal.BuildCuts();
	Analysis anal_nom = anal;

	// flatten cuts if needed
	if (cuts_flat)
	{
		anal_nom.fc_L_l.th_x_m = anal_nom.fc_L_l.th_x_p = anal_nom.fc_L_l.al_m = anal_nom.fc_L_l.al_p = 0.;
		anal_nom.fc_L_h.th_x_m = anal_nom.fc_L_h.th_x_p = anal_nom.fc_L_h.al_m = anal_nom.fc_L_h.al_p = 0.;

		anal_nom.fc_R_l.th_x_m = anal_nom.fc_R_l.th_x_p = anal_nom.fc_R_l.al_m = anal_nom.fc_R_l.al_p = 0.;
		anal_nom.fc_R_h.th_x_m = anal_nom.fc_R_h.th_x_p = anal_nom.fc_R_h.al_m = anal_nom.fc_R_h.al_p = 0.;

		anal_nom.fc_G_l.th_x_m = anal_nom.fc_G_l.th_x_p = anal_nom.fc_G_l.al_m = anal_nom.fc_G_l.al_p = 0.;
		anal_nom.fc_G_h.th_x_m = anal_nom.fc_G_h.th_x_p = anal_nom.fc_G_h.al_m = anal_nom.fc_G_h.al_p = 0.;
	}

	// print environment
	printf("-------------------- env_nom --------------------\n");
	env_nom.Print();

	printf("-------------------- anal_nom --------------------\n");
	anal_nom.Print();
	printf("\n");

	// initialise acceptance calculation
	AcceptanceCalculator accCalc;
	accCalc.Init(th_y_sign, anal_nom);

	// load simuluation model
	TFile *f_dsdt = TFile::Open("/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/first_fits/2/do_fit.root");
	TGraph *g_dsdt = (TGraph *) f_dsdt->Get("variant 2/g_dsdt_CH");
	TSpline *s_dsdt = BuildSpline(g_dsdt);

	// simulation settings
	double be = 6.;
	double ga = 1. - exp(-be * t_max_full);

	printf("t_max_full = %E\n", t_max_full);
	printf("be = %E\n", be);
	printf("ga = %E\n", ga);

	// prepare output
	TFile *f_out = TFile::Open(outputFileName.c_str(), "recreate");

	// prepare histograms for smearing control
	TH2D *h_th_x_d_vs_th_x_m_full = new TH2D("h_th_x_d_vs_th_x_m_full", ";th_x_m;th_x_d", 100, -1.5E-6, +1.5E-6, 100, -70E-6, +70E-6);
	TH2D *h_th_y_d_vs_th_y_m_full = new TH2D("h_th_y_d_vs_th_y_m_full", ";th_y_m;th_y_d", 100, -1E-6, +1E-6, 100, -2E-6, +2E-6);

	TH2D *h_th_x_d_vs_th_x_m_acc = new TH2D("h_th_x_d_vs_th_x_m_acc", ";th_x_m;th_x_d", 100, -1.5E-6, +1.5E-6, 100, -70E-6, +70E-6);
	TH2D *h_th_y_d_vs_th_y_m_acc = new TH2D("h_th_y_d_vs_th_y_m_acc", ";th_y_m;th_y_d", 100, -1E-6, +1E-6, 100, -2E-6, +2E-6);

	// prepare statistics objects
	Stat stat_th_x_full(2), stat_th_x_acc(2);
	stat_th_x_full.SetLabels({"m_x", "d_x"});
	stat_th_x_acc.SetLabels({"m_x", "d_x"});

	Stat stat_th_y_full(2), stat_th_y_acc(2);
	stat_th_y_full.SetLabels({"m_y", "d_y"});
	stat_th_y_acc.SetLabels({"m_y", "d_y"});

	// simulation loop
	for (unsigned long ev = 0; ev < N_ev; ev++)
	{
		if (debug)
		{
			printf("\n");
			printf("--------------------------------------------------------\n");
			printf("event %lu\n", ev);
		}

		// ----- generate (true) elastic event -----

		Kinematics k_tr;

		double u = gRandom->Rndm();
		k_tr.t = - log(1. - ga * u) / be;
		double w = s_dsdt->Eval(k_tr.t) / exp(-be * k_tr.t);

		k_tr.phi = th_y_sign * gRandom->Rndm() * M_PI;

		k_tr.TPhiToThetas(env_nom);

		if (debug)
		{
			printf("k_tr: t = %E\n", k_tr.t);

			printf("k_tr: th_x_L = %E, th_x_R = %E, th_x = %E; th_y_L = %E, th_y_R = %E, th_y = %E\n",
				k_tr.th_x_L, k_tr.th_x_R, k_tr.th_x,
				k_tr.th_y_L, k_tr.th_y_R, k_tr.th_y);
		}

		// ----- generate beam divergence and vertex -----

		Kinematics k_sm = k_tr;

		k_sm.th_x_R += gRandom->Gaus() * env_nom.si_th_x_R;
		k_sm.th_x_L += gRandom->Gaus() * env_nom.si_th_x_L;

		k_sm.th_y_R += gRandom->Gaus() * env_nom.si_th_y_R;
		k_sm.th_y_L += gRandom->Gaus() * env_nom.si_th_y_L;

		k_sm.vtx_x = gRandom->Gaus() * env_nom.si_vtx_x;
		k_sm.vtx_y = gRandom->Gaus() * env_nom.si_vtx_y;

		if (debug)
		{
			printf("k_sm: th_x_L = %E, th_x_R = %E, th_x = %E; th_y_L = %E, th_y_R = %E, th_y = %E\n",
				k_sm.th_x_L, k_sm.th_x_R, k_sm.th_x,
				k_sm.th_y_L, k_sm.th_y_R, k_sm.th_y);
		}

		// ----- proton transport -----

		HitData h_orig = ProtonTransport(k_sm, env_nom);

		// ----- sensor smearing -----

		HitData h_sm = h_orig;

		h_sm.L_2_F.x += gRandom->Gaus() * env_nom.si_de_P_L; 
		h_sm.L_2_F.y += gRandom->Gaus() * env_nom.si_de_P_L; 

		//h_sm.L_1_F.x += gRandom->Gaus() * env_nom.si_de_P_L;
		//h_sm.L_1_F.y += gRandom->Gaus() * env_nom.si_de_P_L;

		//h_sm.R_1_F.x += gRandom->Gaus() * env_nom.si_de_P_R;
		//h_sm.R_1_F.y += gRandom->Gaus() * env_nom.si_de_P_R;

		h_sm.R_2_F.x += gRandom->Gaus() * env_nom.si_de_P_R; 
		h_sm.R_2_F.y += gRandom->Gaus() * env_nom.si_de_P_R; 

		// ----- reconstruction -----

		Kinematics k_re = DoReconstruction(h_sm, env_nom);

		if (debug)
		{
			printf("k_re: th_x_L = %E, th_x_R = %E, th_x = %E; th_y_L = %E, th_y_R = %E, th_y = %E\n",
				k_re.th_x_L, k_re.th_x_R, k_re.th_x,
				k_re.th_y_L, k_re.th_y_R, k_re.th_y);
		}

		// ----- fiducial cuts -----

		double corr_phi = 0.;
		bool skip = false;
		if (acceptance_algorithm_new)
			skip = accCalc.PhiComponentCut(k_re.th_x, k_re.th_y);
		else
			skip = CalculateAcceptanceCorrectionPhi(th_y_sign, k_re, anal_nom, corr_phi);

		corr_phi /= 2.;	// simulated just one hemishpere

		// ----- evaluate smearing -----

		double de_th_x_L = k_re.th_x_L - k_tr.th_x_L;
		double de_th_x_R = k_re.th_x_R - k_tr.th_x_R;
		double de_th_x_m = (de_th_x_L + de_th_x_R) / 2.;
		double de_th_x_d = de_th_x_R - de_th_x_L;

		double de_th_y_L = k_re.th_y_L - k_tr.th_y_L;
		double de_th_y_R = k_re.th_y_R - k_tr.th_y_R;
		double de_th_y_m = (de_th_y_L + de_th_y_R) / 2.;
		double de_th_y_d = de_th_y_R - de_th_y_L;

		// ----- fill plots -----

		h_th_x_d_vs_th_x_m_full->Fill(de_th_x_m, de_th_x_d, w);
		h_th_y_d_vs_th_y_m_full->Fill(de_th_y_m, de_th_y_d, w);

		stat_th_x_full.Fill(de_th_x_m, de_th_x_d);
		stat_th_y_full.Fill(de_th_y_m, de_th_y_d);

		if (!skip)
		{
			h_th_x_d_vs_th_x_m_acc->Fill(de_th_x_m, de_th_x_d, w);
			h_th_y_d_vs_th_y_m_acc->Fill(de_th_y_m, de_th_y_d, w);

			stat_th_x_acc.Fill(de_th_x_m, de_th_x_d);
			stat_th_y_acc.Fill(de_th_y_m, de_th_y_d);
		}

	}

	// print statistics
	printf("\n* stat_th_x_full\n");
	stat_th_x_full.PrintStat();
	stat_th_x_full.PrintMeanAndStdDev();
	stat_th_x_full.PrintCorrelation();

	printf("\n* stat_th_x_acc\n");
	stat_th_x_acc.PrintStat();
	stat_th_x_acc.PrintMeanAndStdDev();
	stat_th_x_acc.PrintCorrelation();

	printf("\n* stat_th_y_full\n");
	stat_th_y_full.PrintStat();
	stat_th_y_full.PrintMeanAndStdDev();
	stat_th_y_full.PrintCorrelation();

	printf("\n* stat_th_y_acc\n");
	stat_th_y_acc.PrintStat();
	stat_th_y_acc.PrintMeanAndStdDev();
	stat_th_y_acc.PrintCorrelation();

	// save plots
	gDirectory = f_out;

	h_th_x_d_vs_th_x_m_full->Write();
	h_th_y_d_vs_th_y_m_full->Write();

	h_th_x_d_vs_th_x_m_acc->Write();
	h_th_y_d_vs_th_y_m_acc->Write();

	// clean up
	delete f_out;

	return 0;
}
