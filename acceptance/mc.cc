#include "TRandom2.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

#include "common_definitions.h"
#include "common_algorithms.h"
#include "AcceptanceCalculator.h"

#include "parameters.h"


int main()
{
	// settings
	unsigned int seed = 1;
	unsigned long N_ev = 1E7;

	double B = 20.;	// GeV^-2

	bool debug = false;

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

	printf("-------------------- env_nom --------------------\n");
	env_nom.Print();

	// initialise analysis objects
	anal.BuildCuts();
	anal.n_si = 4.;
	Analysis anal_nom = anal;

	printf("-------------------- anal_nom --------------------\n");
	anal_nom.Print();
	printf("\n");

	// initialise acceptance calculation
	AcceptanceCalculator accCalc;
	accCalc.Init(th_y_sign, anal);

	// simulation t-range
	double t_min_full = anal.t_min_full;
	double t_max_full = anal.t_max_full;
	double t_range = t_max_full - t_min_full;

	printf("t_min_full = %E\n", t_min_full);
	printf("t_max_full = %E\n", t_max_full);

	// prepare output
	TFile *f_out = TFile::Open("mc.root", "recreate");

	// prepare t-histograms
	unsigned int N_bins = 100;
	TH1D *h_t_true, *h_t_sm, *h_t_sm_cut_corr_old, *h_t_sm_cut_corr_new;
	h_t_true = new TH1D("h_t_true", ";|t|   (GeV^{2})", N_bins, t_min_full, t_max_full); h_t_true->Sumw2();
	h_t_sm = new TH1D("h_t_sm", ";|t|   (GeV^{2})", N_bins, t_min_full, t_max_full); h_t_sm->Sumw2();
	h_t_sm_cut_corr_old = new TH1D("h_t_sm_cut_corr_old", ";|t|   (GeV^{2})", N_bins, t_min_full, t_max_full); h_t_sm_cut_corr_old->Sumw2();
	h_t_sm_cut_corr_new = new TH1D("h_t_sm_cut_corr_new", ";|t|   (GeV^{2})", N_bins, t_min_full, t_max_full); h_t_sm_cut_corr_new->Sumw2();

	unsigned int N_bins_lowt = 50;
	double t_max_lowt = 0.01;
	TH1D *h_t_lowt_true, *h_t_lowt_sm, *h_t_lowt_sm_cut_corr_old, *h_t_lowt_sm_cut_corr_new;
	h_t_lowt_true = new TH1D("h_t_lowt_true", ";|t|   (GeV^{2})", N_bins_lowt, 0., t_max_lowt); h_t_lowt_true->Sumw2();
	h_t_lowt_sm = new TH1D("h_t_lowt_sm", ";|t|   (GeV^{2})", N_bins_lowt, 0., t_max_lowt); h_t_lowt_sm->Sumw2();
	h_t_lowt_sm_cut_corr_old = new TH1D("h_t_lowt_sm_cut_corr_old", ";|t|   (GeV^{2})", N_bins_lowt, 0., t_max_lowt); h_t_lowt_sm_cut_corr_old->Sumw2();
	h_t_lowt_sm_cut_corr_new = new TH1D("h_t_lowt_sm_cut_corr_new", ";|t|   (GeV^{2})", N_bins_lowt, 0., t_max_lowt); h_t_lowt_sm_cut_corr_new->Sumw2();

	// prepare th histograms
	TH2D *h_th_x_th_y_true = new TH2D("h_th_x_th_y_true", ";#theta_{x};#theta_{y}", 300, -300E-6, +300E-6, 200, -100E-6, +100E-6);
	TH2D *h_th_x_th_y_sm = new TH2D("h_th_x_th_y_sm", ";#theta_{x};#theta_{y}", 300, -300E-6, +300E-6, 200, -100E-6, +100E-6);
	TH2D *h_th_x_th_y_sm_cut_corr_old = new TH2D("h_th_x_th_y_sm_cut_corr_old", ";#theta_{x};#theta_{y}", 300, -300E-6, +300E-6, 200, -100E-6, +100E-6);
	TH2D *h_th_x_th_y_sm_cut_corr_new = new TH2D("h_th_x_th_y_sm_cut_corr_new", ";#theta_{x};#theta_{y}", 300, -300E-6, +300E-6, 200, -100E-6, +100E-6);

	// simulation loop
	for (unsigned long ev = 0; ev < N_ev; ev++)
	{
		if (debug)
		{
			printf("\n");
			printf("event %lu\n", ev);
		}

		// ----- generate (true) elastic event -----

		Kinematics k_tr;
		k_tr.t = gRandom->Rndm() * t_range + t_min_full;
		k_tr.phi = th_y_sign * gRandom->Rndm() * 2. * M_PI;
		double w = exp(-B * k_tr.t);

		k_tr.TPhiToThetas(env_nom);

		if (debug)
		{
			printf("k_tr: t = %E\n", k_tr.t);

			printf("k_tr: th_x_L = %E, th_x_R = %E, th_x = %E; th_y_L = %E, th_y_R = %E, th_y = %E\n",
				k_tr.th_x_L, k_tr.th_x_R, k_tr.th_x,
				k_tr.th_y_L, k_tr.th_y_R, k_tr.th_y);
		}

		// ----- generate beam smearing -----

		double rg_x_R = gRandom->Gaus();
		double rg_x_L = gRandom->Gaus();
		double rg_y_R = gRandom->Gaus();
		double rg_y_L = gRandom->Gaus();
		
		Kinematics k_sm = k_tr;

		k_sm.th_x_R += rg_x_R * anal_nom.si_th_x_1arm_R;
		k_sm.th_x_L += rg_x_L * anal_nom.si_th_x_1arm_L;
		k_sm.th_y_R += rg_y_R * anal_nom.si_th_y_1arm;
		k_sm.th_y_L += rg_y_L * anal_nom.si_th_y_1arm;

		k_sm.th_x = (k_sm.th_x_R + k_sm.th_x_L) / 2.;
		k_sm.th_y = (k_sm.th_y_R + k_sm.th_y_L) / 2.;

		k_sm.ThetasToTPhi(env_nom);

		if (debug)
		{
			printf("k_sm: th_x_L = %E, th_x_R = %E, th_x = %E; th_y_L = %E, th_y_R = %E, th_y = %E\n",
				k_sm.th_x_L, k_sm.th_x_R, k_sm.th_x,
				k_sm.th_y_L, k_sm.th_y_R, k_sm.th_y);
		}

		// ----- acceptance correction -----

		double phi_corr_old = 0., div_corr_old = 0.;
		bool skip_old = CalculateAcceptanceCorrections(th_y_sign, k_sm, anal_nom, phi_corr_old, div_corr_old);

		double phi_corr_new = 0., div_corr_new = 0.;
		bool skip_new = accCalc.Calculate(k_sm, phi_corr_new, div_corr_new);

		if (debug)
		{
			printf("old: skip = %u, phi_corr = %.3f, dif_corr = %.3f\n", skip_old, phi_corr_old, div_corr_old);
			printf("new: skip = %u, phi_corr = %.3f, dif_corr = %.3f\n", skip_new, phi_corr_new, div_corr_new);
		}

		// ----- fill plots -----

		h_t_true->Fill(k_tr.t, w);
		h_t_lowt_true->Fill(k_tr.t, w);
		h_th_x_th_y_true->Fill(k_tr.th_x, k_tr.th_y, w);

		h_t_sm->Fill(k_sm.t, w);
		h_t_lowt_sm->Fill(k_sm.t, w);
		h_th_x_th_y_sm->Fill(k_sm.th_x, k_sm.th_y, w);

		if (!skip_old)
		{
			h_t_sm_cut_corr_old->Fill(k_sm.t, w * div_corr_old * phi_corr_old);
			h_t_lowt_sm_cut_corr_old->Fill(k_sm.t, w * div_corr_old * phi_corr_old);
			h_th_x_th_y_sm_cut_corr_old->Fill(k_sm.th_x, k_sm.th_y, w * div_corr_old);
		}

		if (!skip_new)
		{
			h_t_sm_cut_corr_new->Fill(k_sm.t, w * div_corr_new * phi_corr_new);
			h_t_lowt_sm_cut_corr_new->Fill(k_sm.t, w * div_corr_new * phi_corr_new);
			h_th_x_th_y_sm_cut_corr_new->Fill(k_sm.th_x, k_sm.th_y, w * div_corr_new);
		}
	}

	// normalise histograms
	h_t_true->Scale(t_range / N_ev, "width");
	h_t_sm->Scale(t_range / N_ev, "width");
	h_t_sm_cut_corr_old->Scale(t_range / N_ev, "width");
	h_t_sm_cut_corr_new->Scale(t_range / N_ev, "width");

	h_t_lowt_true->Scale(t_max_lowt / N_ev, "width");
	h_t_lowt_sm->Scale(t_max_lowt / N_ev, "width");
	h_t_lowt_sm_cut_corr_old->Scale(t_max_lowt / N_ev, "width");
	h_t_lowt_sm_cut_corr_new->Scale(t_max_lowt / N_ev, "width");

	// save plots
	h_t_true->Write();
	h_t_sm->Write();
	h_t_sm_cut_corr_old->Write();
	h_t_sm_cut_corr_new->Write();

	h_t_lowt_true->Write();
	h_t_lowt_sm->Write();
	h_t_lowt_sm_cut_corr_old->Write();
	h_t_lowt_sm_cut_corr_new->Write();

	h_th_x_th_y_true->Write();
	h_th_x_th_y_sm->Write();
	h_th_x_th_y_sm_cut_corr_old->Write();
	h_th_x_th_y_sm_cut_corr_new->Write();

	// clean up
	delete f_out;

	return 0;
}
