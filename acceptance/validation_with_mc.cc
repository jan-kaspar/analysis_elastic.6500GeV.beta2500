#include "TRandom2.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TSpline.h"
#include "TMatrixDSymEigen.h"

#include "common_definitions.h"
#include "common_algorithms.h"
#include "AcceptanceCalculator.h"

#include "parameters_mc.h"

//----------------------------------------------------------------------------------------------------

TSpline* BuildSpline(TGraph *g)
{
	TSpline3 *s = new TSpline3("", g->GetX(), g->GetY(), g->GetN());
	s->SetName(g->GetName());
	return s;
}

//----------------------------------------------------------------------------------------------------

TMatrixD BuildGeneratorMatrix(double si_m, double si_d, double rho)
{
	// covariance matrix
	TMatrixDSym V(2);
	V(0, 0) = si_m * si_m; V(0, 1) = si_m * si_d * rho;
	V(1, 0) = si_m * si_d * rho; V(1, 1) = si_d * si_d;

	// eigen decomposition
	TMatrixDSymEigen eig_decomp(V);
	TVectorD eig_values(eig_decomp.GetEigenValues());

	// diagonal matrix with square roots of eigenvalues
	TMatrixDSym S(2);
	for (unsigned int i = 0; i < 2; i++)
		S(i, i) = (eig_values(i) >= 0.) ? sqrt(eig_values(i)) : 0.;

	// return generator matrix
	return eig_decomp.GetEigenVectors() * S;
}

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: validation_with_mc [options]\n");
	printf("OPTIONS:\n");
	printf("    -debug         run in verbose mode\n");
	printf("    -seed <int>    set seed of random generator\n");
	printf("    -events <int>  set number of events\n");
	printf("    -t-max <val>   set maximum value of |t|\n");
	printf("    -md-corr       use correlation of m and d\n");
	printf("    -md-uncorr     do not use correlation of m and d\n");
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

	bool md_correlation = true;

	bool cuts_flat = false;

	bool acceptance_algorithm_new = true;

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

		if (strcmp(argv[i], "-md-corr") == 0)
		{
			md_correlation = true;
			continue;
		}

		if (strcmp(argv[i], "-md-uncorr") == 0)
		{
			md_correlation = false;
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
	printf("\tmd_correlation = %u\n", md_correlation);
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
	printf("\n");
	printf("-------------------- env_nom --------------------\n");
	env_nom.Print();

	printf("-------------------- anal_nom --------------------\n");
	anal_nom.Print();

	printf("--------------------------------------------------\n");
	printf("\n");

	// initialise acceptance calculation
	AcceptanceCalculator accCalc;
	accCalc.Init(th_y_sign, anal_nom);

	// load simuluation model
	TFile *f_dsdt = TFile::Open("/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/second_fit/do_fit.root");
	TGraph *g_dsdt = (TGraph *) f_dsdt->Get("variant 2/g_dsdt_CH");
	TSpline *s_dsdt = BuildSpline(g_dsdt);

	// simulation settings
	double be = 6.;
	double ga = 1. - exp(-be * t_max_full);

	printf("t_max_full = %E\n", t_max_full);
	printf("be = %E\n", be);
	printf("ga = %E\n", ga);

	// correlation settings
	const double md_rho_x = (md_correlation) ? 0.2 : 0.;
	const double md_rho_y = (md_correlation) ? 0.02 : 0.;

	printf("md_rho_x = %E\n", md_rho_x);
	printf("md_rho_y = %E\n", md_rho_y);

	// build generator matrices
	const TMatrixD gm_x = BuildGeneratorMatrix(anal_nom.si_th_x_2arm, anal_nom.si_th_x_LRdiff, md_rho_x);
	const TMatrixD gm_y = BuildGeneratorMatrix(anal_nom.si_th_y_2arm, anal_nom.si_th_y_LRdiff, md_rho_y);

	printf("* gm_x:\n");
	gm_x.Print();

	printf("* gm_y:\n");
	gm_y.Print();

	// prepare output
	TFile *f_out = TFile::Open(outputFileName.c_str(), "recreate");

	// list of binnings
	vector<string> binnings;
	binnings.push_back("ob-1-20-0.05");
	//binnings.push_back("ob-2-10-0.05");
	binnings.push_back("ob-3-5-0.05");

	// theta_x ranges
	vector< pair<double, double> > th_x_ranges = {
		{ -1E0, +1E0 },
		{ -0.5E-6, +0.5E-6 },
		{ 9.5E-6, 10.5E-6 },
		{ -31E-6, -30E-6 },
		{ +30E-6, +31E-6 },
	};

	// prepare t-histograms
	map<unsigned int, TH1D*> bh_t_tr, bh_t_re_no_acc, bh_t_re_acc_phi, bh_t_re_acc;
	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		unsigned int N_bins;
		double *bin_edges;
		BuildBinning(anal_nom, binnings[bi], bin_edges, N_bins);

		bh_t_tr[bi] = new TH1D("", ";|t|;events per bin", N_bins, bin_edges); bh_t_tr[bi]->Sumw2();
		bh_t_re_no_acc[bi] = new TH1D("", ";|t|;events per bin", N_bins, bin_edges); bh_t_re_no_acc[bi]->Sumw2();
		bh_t_re_acc_phi[bi] = new TH1D("", ";|t|;events per bin", N_bins, bin_edges); bh_t_re_acc_phi[bi]->Sumw2();
		bh_t_re_acc[bi] = new TH1D("", ";|t|;events per bin", N_bins, bin_edges); bh_t_re_acc[bi]->Sumw2();
	}

	// prepare th_y vs. th_x histograms
	TH2D *h_th_x_th_y_true = new TH2D("h_th_x_th_y_true", ";#theta_{x};#theta_{y}", 160, -200E-6, +200E-6, 960, -120E-6, +120E-6);
	TH2D *h_th_x_th_y_re = new TH2D("h_th_x_th_y_re", ";#theta_{x};#theta_{y}", 160, -200E-6, +200E-6, 960, -120E-6, +120E-6);
	TH2D *h_th_x_th_y_re_cut_corr = new TH2D("h_th_x_th_y_re_cut_corr", ";#theta_{x};#theta_{y}", 160, -200E-6, +200E-6, 960, -120E-6, +120E-6);

	// prepare th_y histograms
	vector<TH1D *> v_h_th_y_re, v_h_th_y_re_cut, v_h_th_y_re_cut_corr;
	for (unsigned int i = 0; i < th_x_ranges.size(); ++i)
	{
		const double th_y_min = 3E-6, th_y_max = 8E-6;
		v_h_th_y_re.push_back(new TH1D("", "#theta_{y}", 200, th_y_min, th_y_max));
		v_h_th_y_re_cut.push_back(new TH1D("", "#theta_{y}", 200, th_y_min, th_y_max));
		v_h_th_y_re_cut_corr.push_back(new TH1D("", "#theta_{y}", 200, th_y_min, th_y_max));
	}

	// prepare histograms for smearing control
	TH2D *h_th_x_d_vs_th_x_m_full = new TH2D("h_th_x_d_vs_th_x_m_full", ";th_x_m;th_x_d", 100, -1.5E-6, +1.5E-6, 100, -70E-6, +70E-6);
	TH2D *h_th_x_d_vs_th_x_m_acc = new TH2D("h_th_x_d_vs_th_x_m_acc", ";th_x_m;th_x_d", 100, -1.5E-6, +1.5E-6, 100, -70E-6, +70E-6);
	TH2D *h_th_y_d_vs_th_y_m_full = new TH2D("h_th_y_d_vs_th_y_m_full", ";th_y_m;th_y_d", 100, -1E-6, +1E-6, 100, -2E-6, +2E-6);
	TH2D *h_th_y_d_vs_th_y_m_acc = new TH2D("h_th_y_d_vs_th_y_m_acc", ";th_y_m;th_y_d", 100, -1E-6, +1E-6, 100, -2E-6, +2E-6);

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

		k_tr.TPhiToThetas(env_nom);	// calculates th, th_x, th_y from t and phi

		if (debug)
		{
			printf("k_tr: t = %E\n", k_tr.t);

			printf("k_tr: th_x_L = %E, th_x_R = %E, th_x = %E; th_y_L = %E, th_y_R = %E, th_y = %E\n",
				k_tr.th_x_L, k_tr.th_x_R, k_tr.th_x,
				k_tr.th_y_L, k_tr.th_y_R, k_tr.th_y);
		}

		// ----- add smearing -----

		Kinematics k_re;

		TVectorD gv_x(2), sv_x(2);
		gv_x(0) = gRandom->Gaus();
		gv_x(1) = gRandom->Gaus();
		sv_x = gm_x * gv_x;

		TVectorD gv_y(2), sv_y(2);
		gv_y(0) = gRandom->Gaus();
		gv_y(1) = gRandom->Gaus();
		sv_y = gm_y * gv_y;

		const double &m_x = sv_x(0);
		const double &d_x = sv_x(1);

		const double &m_y = sv_y(0);
		const double &d_y = sv_y(1);

		k_re.th_x_L = k_tr.th_x_L + m_x - d_x/2.;
		k_re.th_x_R = k_tr.th_x_R + m_x + d_x/2.;
                                    
		k_re.th_y_L = k_tr.th_y_L + m_y - d_y/2.;
		k_re.th_y_R = k_tr.th_y_R + m_y + d_y/2.;

		k_re.th_x = (k_re.th_x_L + k_re.th_x_R) / 2.;
		k_re.th_y = (k_re.th_y_L + k_re.th_y_R) / 2.;

		k_re.ThetasToTPhi(env_nom);	 // calculates th and t from th_x and th_y

		// ----- acceptance correction -----

		double corr_phi = 0., corr_smear = 0.;
		bool skip_smear, skip_phi;
		if (acceptance_algorithm_new)
		{
			skip_smear = accCalc.SmearingComponentCut(k_re.th_x_L, k_re.th_x_R, k_re.th_y_L, k_re.th_y_R);
			skip_phi = accCalc.PhiComponentCut(k_re.th_x, k_re.th_y);

			const double F = accCalc.SmearingFactor(k_re.th_x, k_re.th_y);
			corr_smear = 1. / F;

			corr_phi = accCalc.PhiFactor(k_re.th);
		} else {
			skip_smear = CalculateAcceptanceCorrectionSmearing(th_y_sign, k_re, anal_nom, corr_smear);
			skip_phi = CalculateAcceptanceCorrectionPhi(th_y_sign, k_re, anal_nom, corr_phi);
		}

		corr_phi /= 2.;	// simulated just one hemishpere

		bool skip = (skip_smear || skip_phi);

		if (debug)
		{
			printf("skip = %u, corr_phi = %.3f, dif_corr = %.3f\n", skip, corr_phi, corr_smear);
		}

		// ----- fill plots -----

		for (unsigned int bi = 0; bi < binnings.size(); ++bi)
		{
			bh_t_tr[bi]->Fill(k_tr.t, w);
			bh_t_re_no_acc[bi]->Fill(k_re.t, w);
		}

		h_th_x_th_y_true->Fill(k_tr.th_x, k_tr.th_y, w);
		h_th_x_th_y_re->Fill(k_re.th_x, k_re.th_y, w);

		h_th_x_d_vs_th_x_m_full->Fill(m_x, d_x);
		h_th_y_d_vs_th_y_m_full->Fill(m_y, d_y);

		if (!skip_phi)
		{
			for (unsigned int bi = 0; bi < binnings.size(); ++bi)
			{
				bh_t_re_acc_phi[bi]->Fill(k_re.t, w * corr_phi);
			}
		}

		if (!skip)
		{
			for (unsigned int bi = 0; bi < binnings.size(); ++bi)
			{
				bh_t_re_acc[bi]->Fill(k_re.t, w * corr_smear * corr_phi);
			}

			h_th_x_th_y_re_cut_corr->Fill(k_re.th_x, k_re.th_y, w * corr_smear);

			h_th_x_d_vs_th_x_m_acc->Fill(m_x, d_x);
			h_th_y_d_vs_th_y_m_acc->Fill(m_y, d_y);
		}
		
		for (unsigned int i = 0; i < th_x_ranges.size(); ++i)
		{
			if (k_re.th_x > th_x_ranges[i].first && k_re.th_x < th_x_ranges[i].second)
			{
				v_h_th_y_re[i]->Fill(k_re.th_y);

				if (!skip)
				{
					v_h_th_y_re_cut[i]->Fill(k_re.th_y);
					v_h_th_y_re_cut_corr[i]->Fill(k_re.th_y, corr_smear);
				}
			}
		}
	}

	// normalise histograms
	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		bh_t_tr[bi]->Scale(1./N_ev*t_max_full, "width");
		bh_t_re_no_acc[bi]->Scale(1./N_ev*t_max_full, "width");
		bh_t_re_acc_phi[bi]->Scale(1./N_ev*t_max_full, "width");
		bh_t_re_acc[bi]->Scale(1./N_ev*t_max_full, "width");
	}

	// save plots
	for (unsigned int bi = 0; bi < binnings.size(); ++bi)
	{
		gDirectory = f_out->mkdir(binnings[bi].c_str());

		bh_t_tr[bi]->SetLineColor(4);
		bh_t_tr[bi]->Write("h_t_tr");

		bh_t_re_no_acc[bi]->SetLineColor(8);
		bh_t_re_no_acc[bi]->Write("h_t_re_no_acc");

		bh_t_re_acc_phi[bi]->SetLineColor(6);
		bh_t_re_acc_phi[bi]->Write("h_t_re_acc_phi");

		bh_t_re_acc[bi]->SetLineColor(2);
		bh_t_re_acc[bi]->Write("h_t_re_acc");

		// make ratios
		TH1D *h_t_ratio_tr_over_re_no_acc = new TH1D(* bh_t_tr[bi]);
		h_t_ratio_tr_over_re_no_acc->Divide(bh_t_re_no_acc[bi]);
		h_t_ratio_tr_over_re_no_acc->Write("h_t_ratio_tr_over_re_no_acc");

		TH1D *h_t_ratio_tr_over_re_acc_phi = new TH1D(* bh_t_tr[bi]);
		h_t_ratio_tr_over_re_acc_phi->Divide(bh_t_re_acc_phi[bi]);
		h_t_ratio_tr_over_re_acc_phi->Write("h_t_ratio_tr_over_re_acc_phi");

		TH1D *h_t_ratio_tr_over_re_acc = new TH1D(* bh_t_tr[bi]);
		h_t_ratio_tr_over_re_acc->Divide(bh_t_re_acc[bi]);
		h_t_ratio_tr_over_re_acc->Write("h_t_ratio_tr_over_re_acc");
	}

	gDirectory = f_out;

	h_th_x_th_y_true->Write();
	h_th_x_th_y_re->Write();
	h_th_x_th_y_re_cut_corr->Write();

	h_th_x_d_vs_th_x_m_full->Write();
	h_th_x_d_vs_th_x_m_acc->Write();
	h_th_y_d_vs_th_y_m_full->Write();
	h_th_y_d_vs_th_y_m_acc->Write();

	TDirectory *d_th_ranges = f_out->mkdir("th_x_ranges");
	
	for (unsigned int i = 0; i < th_x_ranges.size(); ++i)
	{
		char buf[100];
		sprintf(buf, "%+.1E_%+.1E", th_x_ranges[i].first, th_x_ranges[i].second);
		gDirectory = d_th_ranges->mkdir(buf);

		// save histograms
		v_h_th_y_re[i]->Sumw2();
		v_h_th_y_re[i]->Scale(1./N_ev*t_max_full, "width");
		v_h_th_y_re[i]->SetLineColor(8);
		v_h_th_y_re[i]->Write("h_th_y_re");

		v_h_th_y_re_cut[i]->Sumw2();
		v_h_th_y_re_cut[i]->Scale(1./N_ev*t_max_full, "width");
		v_h_th_y_re_cut[i]->SetLineColor(6);
		v_h_th_y_re_cut[i]->Write("h_th_y_re_cut");

		v_h_th_y_re_cut_corr[i]->Sumw2();
		v_h_th_y_re_cut_corr[i]->Scale(1./N_ev*t_max_full, "width");
		v_h_th_y_re_cut_corr[i]->SetLineColor(2);
		v_h_th_y_re_cut_corr[i]->Write("h_th_y_re_cut_corr");

		// make ratios
		TH1D *h_th_ratio_cut_over_no_cut = new TH1D(* v_h_th_y_re_cut[i]);
		h_th_ratio_cut_over_no_cut->Divide(v_h_th_y_re[i]);
		h_th_ratio_cut_over_no_cut->Write("h_th_ratio_cut_over_no_cut");

		TH1D *h_th_ratio_cut_corr_over_no_cut = new TH1D(* v_h_th_y_re_cut_corr[i]);
		h_th_ratio_cut_corr_over_no_cut->Divide(v_h_th_y_re[i]);
		h_th_ratio_cut_corr_over_no_cut->Write("h_th_ratio_cut_corr_over_no_cut");

		// make graphs
		const double th_x_ref = (th_x_ranges[i].first + th_x_ranges[i].second) / 2.;

		TGraph *g_acc_smear_vs_th_y = new TGraph();
		for (double th_y = 3E-6; th_y <= 8E-6; th_y += 0.02E-6)
		{
			double acc = 0.;
			if (acceptance_algorithm_new)
			{
				acc = accCalc.SmearingFactor(th_x_ref, th_y);
			} else {
				Kinematics k;
				k.th_y_L = k.th_y_R = k.th_y = th_y;
				double corr_smear = 0.;
				bool skip = CalculateAcceptanceCorrectionSmearing(th_y_sign, k, anal_nom, corr_smear);
				acc = (skip) ? 0. : 1. / corr_smear;
			}

			int idx = g_acc_smear_vs_th_y->GetN();
			g_acc_smear_vs_th_y->SetPoint(idx, th_y, acc);
		}

		g_acc_smear_vs_th_y->Write("g_acc_smear_vs_th_y");
	}

	// clean up
	delete f_out;

	return 0;
}
