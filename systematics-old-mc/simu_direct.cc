#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TRandom2.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TF1.h"

#include "../common_definitions.h"
#include "../common_algorithms.h"
#include "../AcceptanceCalculator.h"

#include "common.h"

#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

Environment env_nom;
Analysis anal_nom;

//----------------------------------------------------------------------------------------------------

DSType ds = dsUnknown;

DGNType dgn = dgnUnknown;

int Setup(const string &s_dataset, const string &s_diagonal)
{
	// set dataset and diagonal
	ds = GetDataSetType(s_dataset);
	if (ds == dsUnknown)
	{
		printf("ERROR: dataset `%s' not recognized.\n", s_dataset.c_str());
		return 1;
	}

	dgn = GetDiagonalType(s_diagonal);
	if (dgn == dgnUnknown)
	{
		printf("ERROR: diagonal `%s' not recognized.\n", s_diagonal.c_str());
		return 2;
	}

	//--------------------------------------------------

	// sign of th_y
	if (dgn == dgn_45b)
		th_y_sign = +1.;
	else
		th_y_sign = -1.;

	//--------------------------------------------------

	env_nom.InitNominal();

	// binning
	anal_nom.t_min = 8E-4; anal_nom.t_max = 1.0;
	anal_nom.t_min_full = 0.; anal_nom.t_max_full = 1.1;

	// approximate (time independent) resolutions
	anal_nom.si_th_x_LRdiff = 12.5E-6;
	anal_nom.si_th_x_2arm = 0.3E-6;

	anal_nom.si_th_y_LRdiff = 0.37E-6;
	anal_nom.si_th_y_2arm = 0.185E-6;

	// TODO: diagonal dependent ?
	anal_nom.fc_L_l = FiducialCut(4.1E-6, -20E-6, -0.01, +10E-6, +0.05);
	anal_nom.fc_L_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal_nom.fc_R_l = FiducialCut(4.2E-6, -20E-6, -0.01, +20E-6, 0.05);
	anal_nom.fc_R_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal_nom.fc_G_l = FiducialCut(4.3E-6, -20E-6, -0.01, +10E-6, +0.05);
	anal_nom.fc_G_h = FiducialCut(100E-6, 0E-6, 0., 0E-6, 0.);

	//--------------------------------------------------

	env_nom.Print();
	anal_nom.Print();

	return 0;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

unsigned int N_bins;
double *bin_edges;

struct HistGroup
{
	string name;
	TH1D *h_th_x, *h_th_y, *h_th, *h_t;
	TH1D *h_diffRL_th_y;
	TH1D *h_de_th_y_L, *h_de_th_y_R;

	HistGroup(const string _n = "") :
		name(_n)
	{
		h_th_x = new TH1D("", ";#theta_{x}^{*}  (#murad)", 200, -200E-6, 200E-6); h_th_x->Sumw2();
		h_th_y = new TH1D("", ";#theta_{y}^{*}  (#murad)", 100, -100E-6, 100E-6); h_th_y->Sumw2();
		h_th = new TH1D("", ";#theta^{*}  (#murad)", 400, 0E-6, 200E-6); h_th->Sumw2();
		h_t = new TH1D("", ";t  (GeV^{2})", N_bins, bin_edges); h_t->Sumw2();

		h_diffRL_th_y = new TH1D("", ";#Delta^{R-L} #theta_{y}^{*}  (#murad)", 400, -50E-6, 50E-6); h_diffRL_th_y->Sumw2();

		h_de_th_y_L = new TH1D("", ";#Delta^{L-true} #theta_{y}^{*}  (#murad)", 400, -50E-6, 50E-6); h_de_th_y_L->Sumw2();
		h_de_th_y_R = new TH1D("", ";#Delta^{R-true} #theta_{y}^{*}  (#murad)", 400, -50E-6, 50E-6); h_de_th_y_R->Sumw2();
	}

	void Fill(const Kinematics &k, double p, double w, double phi_corr, double div_corr)
	{
		double th2 = k.th_x*k.th_x + k.th_y*k.th_y;
		double th = sqrt(th2);
		double t = p*p * th2;

		h_th_x->Fill(k.th_x, w*div_corr);
		h_th_y->Fill(k.th_y, w*div_corr);
		h_th->Fill(th, w*div_corr*phi_corr);
		h_t->Fill(t, w*div_corr*phi_corr);
	}

	void FillExtra(const Kinematics &k_sm, const Kinematics &k_tr, double w, unsigned long ev)
	{
		h_diffRL_th_y->Fill(k_sm.th_y_R - k_sm.th_y_L, w);

		h_de_th_y_L->Fill(k_sm.th_y_L - k_tr.th_y_L, w);
		h_de_th_y_R->Fill(k_sm.th_y_R - k_tr.th_y_R, w);
	}

	void Write(TH1D *h_t_ref, TH1D *h_th_ref)
	{
		h_t->Scale(1., "width");

		TDirectory *topDir = gDirectory;
		gDirectory = topDir->mkdir(name.c_str());

		TH1D *h_eff_syst = new TH1D(*h_t);
		h_eff_syst->Divide(h_t_ref);
		
		// TODO
		/*
		TH1D *h_eff_syst_crop = new TH1D(*h_eff_syst);
		h_eff_syst_crop->SetLineColor(2);
		int first_bin = h_eff_syst_crop->FindBin(t_min_fit);
		for (int bi = 1; bi < first_bin; bi++)
		{
			h_eff_syst_crop->SetBinContent(bi, 0.);
			h_eff_syst_crop->SetBinError(bi, 0.);
		}

		TH1D *h_eff_syst_th = new TH1D(*h_th);
		h_eff_syst_th->Divide(h_th_ref);
		*/

		h_th_x->Write("h_th_x");
		h_th_y->Write("h_th_y");
		h_th->Write("h_th");

		h_t->Write("h_t");

		h_diffRL_th_y->Write("h_diffRL_th_y");

		h_de_th_y_L->Write("h_de_th_y_L");
		h_de_th_y_R->Write("h_de_th_y_R");

		h_eff_syst->Write("h_eff_syst");

		/*
		h_eff_syst_crop->Write("h_eff_syst_crop");
		h_eff_syst_th->Write("h_eff_syst_th");
		*/

		gDirectory = topDir;
	}
};

//----------------------------------------------------------------------------------------------------

struct Biases
{
	// reconstruction shifts (rad)
	double sh_th_x = 0., sh_th_y = 0.;

	// reconstruction tilt (rad)
	double thx_thy_tilt = 0.;

	// reconstruction scales, th_y_reco = sc_th_y * th_y_true
	double sc_th_x = 1., sc_th_y = 1.;

	// errors of the 3-out-of-4 efficiency plateau slope
	double eff_slope_error = 0.;
	double eff_slope_fix_point = 0.;

	// normalisation error
	double de_norm = 0.;

	/*
	// relative error of beam momentum
	double de_rel_p = 0.;	

	// errors of unsmearing parameters
	double si_d_x_err = 0.;
	double si_d_y_err = 0.;
	*/
};

//----------------------------------------------------------------------------------------------------

Biases biases;

int SetScenario(const string &scenario, Environment &env_sim, Analysis &anal_sim)
{
	// TODO: shift fiducial cuts if needed

	if (scenario.compare("none") == 0)
	{
		return 0;
	}

	if (scenario.compare("alig-sh-thx") == 0)
	{
		biases.sh_th_x = 1E-6;
		return 0;
	}

	if (scenario.compare("alig-sh-thy") == 0)
	{
		biases.sh_th_y = 0.24E-6;
		return 0;
	}

	if (scenario.compare("thx-thy-tilt") == 0)
	{
		biases.thx_thy_tilt = 0.005;
		return 0;
	}

	if (scenario.compare("opt-m1") == 0)
	{
		// TODO: preliminary
		biases.sc_th_x = 1. - 1E-03;
		biases.sc_th_y = 1.;

		return 0;
	}

	if (scenario.compare("opt-m2") == 0)
	{
		// TODO: preliminary
		biases.sc_th_x = 1.;
		biases.sc_th_y = 1. - 1E-03;

		return 0;
	}

	return 1;
}

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: simu_direct <dataset> <diagonal> <scenario> <events> <output file name>\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	if (argc != 6)
	{
		printf("ERROR: wrong number of parameters\n");
		PrintUsage();
		return 1;
	}

	// get command-line arguments
	string dataset = argv[1];
	string diagonal = argv[2];
	string scenario = argv[3];
	unsigned long N_ev = (unsigned long) atof(argv[4]);
	string outFileName = argv[5];

	// set-up for chosen dataset and diagonal
	if (Setup(dataset, diagonal) != 0)
	{
		printf("ERROR: Setup failed.\n");
		return 2;
	}

	// binning
	BuildBinning(anal_nom, "ob-1-20-0.05", bin_edges, N_bins);

	// load input dsigma/dt distribution
	if (LoadTDistributions() != 0)
		return 1;

	TSpline *t_dist = inputTDistributions[0].s_dsdt;

	// output file	
	TFile *out_f = new TFile(outFileName.c_str(), "recreate");

	// random seed
	gRandom->SetSeed(1);
	gRandom->Print();

	// set scenario
	Environment env_sim = env_nom, env_rec = env_nom;
	Analysis anal_sim = anal_nom, anal_rec = anal_nom;

	if (SetScenario(scenario, env_sim, anal_sim) != 0)
	{
		printf("ERROR: scenario '%s' not recognised.\n", scenario.c_str());
	}

	// initialise acceptance calculation
	AcceptanceCalculator accCalc;
	accCalc.Init(th_y_sign, anal_rec);

	// histograms
	HistGroup h_tr("tr"), h_sm("sm"), h_re("re");

	for (unsigned int ev = 0; ev < N_ev; ev++)
	{
		// true event
		Kinematics k_tr;
		k_tr.t = gRandom->Rndm() * 0.3 + 1E-4;
		k_tr.phi = th_y_sign * gRandom->Rndm() * M_PI; // just one hemisphere

		double w = t_dist->Eval(k_tr.t);
	
		k_tr.th = sqrt(k_tr.t) / env_sim.p;
		k_tr.th_x = k_tr.th_x_L = k_tr.th_x_R = k_tr.th * cos(k_tr.phi);
		k_tr.th_y = k_tr.th_y_L = k_tr.th_y_R = k_tr.th * sin(k_tr.phi);
		
		h_tr.Fill(k_tr, env_sim.p, w, 1., 1.);

		// ----- smearing -----

		double f_d_x = gRandom->Gaus();
		double f_d_y = gRandom->Gaus();
		double f_m_x = gRandom->Gaus();
		double f_m_y = gRandom->Gaus();

		double d_x = f_d_x * anal_sim.si_th_x_LRdiff;
		double d_y = f_d_y * anal_sim.si_th_y_LRdiff;

		double m_x = f_m_x * anal_sim.si_th_x_2arm;
		double m_y = f_m_y * anal_sim.si_th_y_2arm;

		Kinematics k_sm;

		k_sm.th_x_R = k_tr.th_x + m_x + d_x/2.;
		k_sm.th_x_L = k_tr.th_x + m_x - d_x/2.;
		k_sm.th_x = (k_sm.th_x_L + k_sm.th_x_R) / 2.;

		k_sm.th_y_R = k_tr.th_y + m_y + d_y/2.;
		k_sm.th_y_L = k_tr.th_y + m_y - d_y/2.;
		k_sm.th_y = (k_sm.th_y_L + k_sm.th_y_R) / 2.;

		h_sm.Fill(k_sm, env_sim.p, w, 1., 1.);


		// ----- reconstruction bias -----

		Kinematics k_re;

		k_re.th_x_L = biases.sc_th_x * (k_sm.th_x_L + biases.sh_th_x) + biases.thx_thy_tilt * k_sm.th_y_L;
		k_re.th_x_R = biases.sc_th_x * (k_sm.th_x_R + biases.sh_th_x) + biases.thx_thy_tilt * k_sm.th_y_R;

		k_re.th_y_L = biases.sc_th_y * (k_sm.th_y_L + biases.sh_th_y);
		k_re.th_y_R = biases.sc_th_y * (k_sm.th_y_R + biases.sh_th_y);

		k_re.th_x = (k_re.th_x_L + k_re.th_x_R) / 2.;
		k_re.th_y = (k_re.th_y_L + k_re.th_y_R) / 2.;
		k_re.th = sqrt(k_re.th_x*k_re.th_x + k_re.th_y*k_re.th_y);


		// ----- fiducial cuts and acceptance correction -----

		double phi_corr = 0., div_corr = 0.;
		bool skip = accCalc.Calculate(k_re, phi_corr, div_corr);

		if (skip)
			continue;

		h_re.Fill(k_re, env_rec.p, w, phi_corr, div_corr / 2.);
	}

	// save
	h_tr.Write(h_tr.h_t, h_tr.h_th);
	h_sm.Write(h_tr.h_t, h_tr.h_th);
	h_re.Write(h_tr.h_t, h_tr.h_th);

	delete out_f;
}
