#include "TRandom2.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

#include "common_definitions.h"
#include "common_algorithms.h"
#include "AcceptanceCalculator.h"

#include "parameters_mc.h"

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: validation_with_mc [options]\n");
	printf("OPTIONS:\n");
	printf("    -debug         run in verbose mode\n");
	printf("    -seed <int>    set seed of random generator\n");
	printf("    -events <int>  set number of events\n");
	printf("    -t-max <val>   set maximum value of |t|\n");
	printf("    -old-acc       use old acceptance calculation engine\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	// defaults
	unsigned int seed = 1;
	unsigned long N_ev = 1E6;

	double B = 20.;	// GeV^-2

	bool debug = false;

	bool cuts_flat = false;

	bool acceptance_algorithm_new = true;

	double t_min_full = 0.;
	double t_max_full = 1.1;

	string outputFileName = "validation_with_mc.root";

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
	printf("\tseed = %u\n", seed);
	printf("\tevents = %lu (%.1E)\n", N_ev, double(N_ev));

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
	anal.n_si = 4.;
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

	// make environment consistent
	anal_nom.si_th_x_LRdiff = 12.95E-6;
	anal_nom.si_th_y_1arm = 0.275E-6;

	// print environment
	printf("-------------------- env_nom --------------------\n");
	env_nom.Print();

	printf("-------------------- anal_nom --------------------\n");
	anal_nom.Print();
	printf("\n");

	// initialise acceptance calculation
	AcceptanceCalculator accCalc;
	accCalc.Init(th_y_sign, anal_nom);

	// simulation t-range
	double t_range = t_max_full - t_min_full;

	printf("t_min_full = %E\n", t_min_full);
	printf("t_max_full = %E\n", t_max_full);

	// prepare output
	TFile *f_out = TFile::Open(outputFileName.c_str(), "recreate");

	// list of binnings
	vector<string> binnings;
	binnings.push_back("ub");
	binnings.push_back("ob-1-30-0.05");

	// prepare t-histograms
	map<unsigned int, TH1D*> bh_t_tr, bh_t_re_no_acc, bh_t_re_acc;
	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		unsigned int N_bins;
		double *bin_edges;
		BuildBinning(anal_nom, binnings[bi], bin_edges, N_bins);

		bh_t_tr[bi] = new TH1D("", ";|t|;events per bin", N_bins, bin_edges); bh_t_tr[bi]->Sumw2();
		bh_t_re_no_acc[bi] = new TH1D("", ";|t|;events per bin", N_bins, bin_edges); bh_t_re_no_acc[bi]->Sumw2();
		bh_t_re_acc[bi] = new TH1D("", ";|t|;events per bin", N_bins, bin_edges); bh_t_re_acc[bi]->Sumw2();
	}

	// prepare th histograms
	TH2D *h_th_x_th_y_true = new TH2D("h_th_x_th_y_true", ";#theta_{x};#theta_{y}", 160, -200E-6, +200E-6, 960, -120E-6, +120E-6);
	TH2D *h_th_x_th_y_re = new TH2D("h_th_x_th_y_re", ";#theta_{x};#theta_{y}", 160, -200E-6, +200E-6, 960, -120E-6, +120E-6);
	TH2D *h_th_x_th_y_re_cut_corr = new TH2D("h_th_x_th_y_re_cut_corr", ";#theta_{x};#theta_{y}", 160, -200E-6, +200E-6, 960, -120E-6, +120E-6);

	// prepare histograms for smearing control
	TH2D *h_th_x_d_vs_th_x_m_full = new TH2D("h_th_x_d_vs_th_x_d_full", ";th_x_m;th_x_d", 100, -1.2E-6, +1.2E-6, 100, -60E-6, +60E-6);
	TH2D *h_th_x_d_vs_th_x_m_acc = new TH2D("h_th_x_d_vs_th_x_d_acc", ";th_x_m;th_x_d", 100, -1.2E-6, +1.2E-6, 100, -60E-6, +60E-6);
	TH2D *h_th_y_d_vs_th_y_m_full = new TH2D("h_th_y_d_vs_th_y_d_full", ";th_y_m;th_y_d", 100, -1E-6, +1E-6, 100, -2E-6, +2E-6);
	TH2D *h_th_y_d_vs_th_y_m_acc = new TH2D("h_th_y_d_vs_th_y_d_acc", ";th_y_m;th_y_d", 100, -1E-6, +1E-6, 100, -2E-6, +2E-6);

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
		k_tr.t = gRandom->Rndm() * t_range + t_min_full;
		k_tr.phi = th_y_sign * gRandom->Rndm() * M_PI;
		double w = exp(-B * k_tr.t);

		k_tr.TPhiToThetas(env_nom);

		if (debug)
		{
			printf("k_tr: t = %E\n", k_tr.t);

			printf("k_tr: th_x_L = %E, th_x_R = %E, th_x = %E; th_y_L = %E, th_y_R = %E, th_y = %E\n",
				k_tr.th_x_L, k_tr.th_x_R, k_tr.th_x,
				k_tr.th_y_L, k_tr.th_y_R, k_tr.th_y);
		}

		// ----- generate beam divergence and vertex -----

		double rg_x_R = gRandom->Gaus();
		double rg_x_L = gRandom->Gaus();
		double rg_y_R = gRandom->Gaus();
		double rg_y_L = gRandom->Gaus();

		Kinematics k_sm = k_tr;

		k_sm.th_x_R += rg_x_R * env_nom.si_th_x_R;
		k_sm.th_x_L += rg_x_L * env_nom.si_th_x_L;

		k_sm.th_y_R += rg_y_R * env_nom.si_th_y_R;
		k_sm.th_y_L += rg_y_L * env_nom.si_th_y_L;

		double rg_vtx_x = gRandom->Gaus();
		double rg_vtx_y = gRandom->Gaus();

		k_sm.vtx_x = rg_vtx_x * env_nom.si_vtx_x;
		k_sm.vtx_y = rg_vtx_y * env_nom.si_vtx_y;

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

		Kinematics k_re;

		k_re.th_x_L = k_re.th_x_L_2_F = - h_sm.L_2_F.x / env_nom.L_x_L_2_F;
		k_re.th_x_R = k_re.th_x_R_2_F = + h_sm.R_2_F.x / env_nom.L_x_R_2_F;
		k_re.th_x = (k_re.th_x_L + k_re.th_x_R) / 2.;

		k_re.th_y_L = k_re.th_y_L_2_F = - h_sm.L_2_F.y / env_nom.L_y_L_2_F;
		k_re.th_y_R = k_re.th_y_R_2_F = + h_sm.R_2_F.y / env_nom.L_y_R_2_F;
		k_re.th_y = (k_re.th_y_L + k_re.th_y_R) / 2.;

		k_re.ThetasToTPhi(env_nom);

		if (debug)
		{
			printf("k_re: th_x_L = %E, th_x_R = %E, th_x = %E; th_y_L = %E, th_y_R = %E, th_y = %E\n",
				k_re.th_x_L, k_re.th_x_R, k_re.th_x,
				k_re.th_y_L, k_re.th_y_R, k_re.th_y);
		}

		// ----- acceptance correction -----

		double phi_corr = 0., div_corr = 0.;
		bool skip = false;
		if (acceptance_algorithm_new)
			accCalc.Calculate(k_re, phi_corr, div_corr);
		else
			CalculateAcceptanceCorrections(th_y_sign, k_re, anal_nom, phi_corr, div_corr);

		phi_corr /= 2.;	// simulated just one hemishpere

		if (debug)
		{
			printf("skip = %u, phi_corr = %.3f, dif_corr = %.3f\n", skip, phi_corr, div_corr);
		}

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

		for (unsigned int bi = 0; bi < binnings.size(); ++bi)
		{
			bh_t_tr[bi]->Fill(k_tr.t, w);
			bh_t_re_no_acc[bi]->Fill(k_re.t, w);
		}

		h_th_x_th_y_true->Fill(k_tr.th_x, k_tr.th_y, w);
		h_th_x_th_y_re->Fill(k_re.th_x, k_re.th_y, w);

		h_th_x_d_vs_th_x_m_full->Fill(de_th_x_m, de_th_x_d);
		h_th_y_d_vs_th_y_m_full->Fill(de_th_y_m, de_th_y_d);

		if (!skip)
		{
			for (unsigned int bi = 0; bi < binnings.size(); ++bi)
			{
				bh_t_re_acc[bi]->Fill(k_re.t, w * div_corr * phi_corr);
			}

			h_th_x_th_y_re_cut_corr->Fill(k_re.th_x, k_re.th_y, w * div_corr);

			h_th_x_d_vs_th_x_m_acc->Fill(de_th_x_m, de_th_x_d);
			h_th_y_d_vs_th_y_m_acc->Fill(de_th_y_m, de_th_y_d);
		}
	}

	// normalise histograms
	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		bh_t_tr[bi]->Scale(1./N_ev, "width");
		bh_t_re_no_acc[bi]->Scale(1./N_ev, "width");
		bh_t_re_acc[bi]->Scale(1./N_ev, "width");
	}

	// save plots
	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		gDirectory = f_out->mkdir(binnings[bi].c_str());

		bh_t_tr[bi]->Write("h_t_tr");
		bh_t_re_no_acc[bi]->Write("h_t_re_no_acc");
		bh_t_re_acc[bi]->Write("h_t_re_acc");
	}

	gDirectory = f_out;

	h_th_x_th_y_true->Write();
	h_th_x_th_y_re->Write();
	h_th_x_th_y_re_cut_corr->Write();

	h_th_x_d_vs_th_x_m_full->Write();
	h_th_x_d_vs_th_x_m_acc->Write();
	h_th_y_d_vs_th_y_m_full->Write();
	h_th_y_d_vs_th_y_m_acc->Write();

	// clean up
	delete f_out;

	return 0;
}
