#ifndef _common_algorithms_h_
#define _common_algorithms_h_

#include "TMath.h"

#include <deque>

#include "common_definitions.h"

//----------------------------------------------------------------------------------------------------

Kinematics DoReconstruction(const HitData &h, const Environment & env)
{
	Kinematics k;

	// single-arm kinematics reconstruction
	k.th_x_L_1_F = 0.;
  	k.th_x_L_2_F = - h.L_2_F.x / env.L_x_L_2_F;
  	k.th_x_L = k.th_x_L_2_F;
  	
	k.th_x_R_1_F = 0.;
  	k.th_x_R_2_F = + h.R_2_F.x / env.L_x_R_2_F;
  	k.th_x_R = k.th_x_R_2_F;

	k.th_y_L_1_F = 0.;
  	k.th_y_L_2_F = - h.L_2_F.y / env.L_y_L_2_F;
  	k.th_y_L = k.th_y_L_2_F;
  	
	k.th_y_R_1_F = 0.;
  	k.th_y_R_2_F = + h.R_2_F.y / env.L_y_R_2_F;
  	k.th_y_R = k.th_y_R_2_F;

  	k.vtx_x_L = k.vtx_x_R = 0.;
  	k.vtx_y_L = k.vtx_y_R = 0.;

	// double-arm kinematics reconstruction
	k.th_x = (k.th_x_L + k.th_x_R) / 2.;
	k.th_y = (k.th_y_L + k.th_y_R) / 2.;
	
	k.vtx_x = 0.;
	k.vtx_y = 0.;

	// theta reconstruction
	double th_sq = k.th_x*k.th_x + k.th_y*k.th_y;
	k.th = sqrt(th_sq);
	k.phi = atan2(k.th_y, k.th_x);

	// t reconstruction
	k.t_x = env.p*env.p * k.th_x * k.th_x;
	k.t_y = env.p*env.p * k.th_y * k.th_y;
	k.t = k.t_x + k.t_y;

	return k;
}

//----------------------------------------------------------------------------------------------------

void BuildBinning(const Analysis &anal, const string &type, double* &binEdges, unsigned int &bins,
		bool verbose = false)
{
	if (verbose)
		printf(">> BuildBinning(%s)\n", type.c_str());
	
	std::vector<double> be;
	double w;

	// same as in the low-|t| analysis
	if (type.compare("ub") == 0)
	{
		w = 0.02;
		double t = 0.;
		while (t < anal.t_max_full)
		{
			be.push_back(t);
			t += w;
		}

		bins = be.size() - 1;
		binEdges = new double[bins + 1];
		for (unsigned int i = 0; i <= bins; i++)
			binEdges[i] = be[i];

		return;
	}

	// between t_min_full and t_min
	unsigned int N_bins_low = 4;
	w = (anal.t_min - anal.t_min_full) / N_bins_low;
	for (unsigned int i = 0; i < N_bins_low; i++)
		be.push_back(anal.t_min_full + w * i);

	// between t_min and t_max
	unsigned int N_bins_cen = 200;
	
	if (type.compare("eb") == 0)
	{
		double B = 3.;
		for (unsigned int bi = 0; bi < N_bins_cen; bi++)
			be.push_back( - log( (1. - double(bi) / N_bins_cen) * exp(-B*anal.t_min) + double(bi) * exp(-B*anal.t_max) / N_bins_cen ) / B );
	}

	if (type.find("ob") == 0)
	{
		// extract parameters
		size_t p1 = type.find("-", 0);
		size_t p2 = type.find("-", p1 + 1);
		size_t p3 = type.find("-", p2 + 1);
		
		double n_smearing_sigmas = atof(type.substr(p1+1, p2-p1-1).c_str());
		string stat_unc_label = type.substr(p2+1, p3-p2-1);
		double bs_max = atof(type.substr(p3+1).c_str());

		// load generators
		TFile *f_in = TFile::Open("/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/beta2500/2rp/binning/generators.root");
		//TFile *f_in = TFile::Open((path_prefix + "../binning/generators.root").c_str());
		TGraph *g_rms_t = (TGraph *) f_in->Get("g_rms_t");
		TGraph *g_bs_fsu = (TGraph *) f_in->Get( ("g_bs_stat_unc_" + stat_unc_label).c_str() );

		double t = anal.t_min;
		while (t < anal.t_max)
		{
			be.push_back(t);

			double w = max(n_smearing_sigmas * g_rms_t->Eval(t), g_bs_fsu->Eval(t));
			double t_c = t + w/2.;
			w = max(n_smearing_sigmas * g_rms_t->Eval(t_c), g_bs_fsu->Eval(t_c));
			if (w > bs_max)
				w = bs_max;

			t += w;
		}

		delete f_in;
	}

	// between t_max and t_max_full
	unsigned int N_bins_high = 4;
	w = (anal.t_max_full - anal.t_max) / N_bins_high;
	for (unsigned int i = 0; i <= N_bins_high; i++)
		be.push_back(anal.t_max + w * i);

	// return results
	bins = be.size() - 1;
	binEdges = new double[be.size()];
	for (unsigned int i = 0; i < be.size(); i++)
	{
		binEdges[i] = be[i];
		if (verbose)
			printf("\tbi = %4u: %.4E\n", i, binEdges[i]);
	}
}

//----------------------------------------------------------------------------------------------------

bool CalculateAcceptanceCorrectionSmearing(double th_y_sign, const Kinematics &k, const Analysis &anal, double &corr_smear)
{
	corr_smear = 0.;

	if ((th_y_sign * k.th_y_L < anal.fc_L_l.th_y_0) || (th_y_sign * k.th_y_R < anal.fc_R_l.th_y_0)
		|| (th_y_sign * k.th_y_L > anal.fc_L_h.th_y_0) || (th_y_sign * k.th_y_R > anal.fc_R_h.th_y_0))
		return true;
	
	const double th_y_abs = th_y_sign * k.th_y;

	const double si_th_y_1arm = anal.si_th_y_LRdiff / sqrt(2.);

	double UB_y = min(anal.fc_R_h.th_y_0 - th_y_abs, th_y_abs - anal.fc_L_l.th_y_0);
	double LB_y = max(anal.fc_R_l.th_y_0 - th_y_abs, th_y_abs - anal.fc_L_h.th_y_0);
	double F_y = (UB_y > LB_y) ? ( TMath::Erf(UB_y / si_th_y_1arm) - TMath::Erf(LB_y / si_th_y_1arm) ) / 2. : 0.;

	corr_smear = 1./ F_y;

	return false;
}

//----------------------------------------------------------------------------------------------------

bool CalculateAcceptanceCorrectionPhi(double th_y_sign, const Kinematics &k, const Analysis &anal, double &corr_phi)
{
	corr_phi = 0.;

	double th_y_lcut = anal.fc_G_l.th_y_0;
	double th_y_hcut = anal.fc_G_h.th_y_0;

	const double th_y_abs = th_y_sign * k.th_y;

	if (th_y_abs <= th_y_lcut || th_y_abs >= th_y_hcut)
		return true;

	// get all intersections
	set<double> phis;

	if (k.th > th_y_lcut)
	{
		double phi = asin(th_y_lcut / k.th);
		phis.insert(phi);
		phis.insert(M_PI - phi);
	}
	
	if (k.th > th_y_hcut)
	{
		double phi = asin(th_y_hcut / k.th);
		phis.insert(phi);
		phis.insert(M_PI - phi);
	}

	// the number of intersections must be even
	if ((phis.size() % 2) == 1)
	{
		printf("ERROR: odd number of intersections in acceptance calculation\n");
	}

	// no intersection => no acceptances
	if (phis.size() == 0)
		return true;

	// calculate arc-length in within acceptance
	double phiSum = 0.;
	for (set<double>::iterator it = phis.begin(); it != phis.end(); ++it)
	{
		double phi_start = *it;
		++it;
		double phi_end = *it;

		phiSum += phi_end - phi_start;
	}
	
	corr_phi = 2. * M_PI / phiSum;

	return false;
}

//----------------------------------------------------------------------------------------------------

bool CalculateAcceptanceCorrections(double th_y_sign, const Kinematics &k, const Analysis &anal, double &corr_phi, double &corr_smear)
{
	bool skip_smear = CalculateAcceptanceCorrectionSmearing(th_y_sign, k, anal, corr_smear);
	if (skip_smear)
		return true;

	bool skip_phi = CalculateAcceptanceCorrectionPhi(th_y_sign, k, anal, corr_phi);
	return skip_phi;
}

//----------------------------------------------------------------------------------------------------

bool SkipRun(unsigned int /*run*/, unsigned int /*file*/, bool /*strict = true */)
{
	return false;
}

//----------------------------------------------------------------------------------------------------

// map: run number (8372) --> list of triggered bunches
typedef std::map<unsigned int, std::vector<unsigned int> > BunchMap;

bool keepAllBunches;
BunchMap bunchMap;

bool SkipBunch(unsigned int run, unsigned bunch)
{
	if (keepAllBunches)
		return false;

	const std::vector<unsigned int> &bunches = bunchMap[run];
	
	return (find(bunches.begin(), bunches.end(), bunch) == bunches.end());
}

//----------------------------------------------------------------------------------------------------

// returns the beam for which the bunch is non-colliding
// for colliding bunches returns zero
unsigned int NonCollidingBunch(unsigned int /*run*/, unsigned /*bunch*/)
{
	/*
	if (run == 8318) {
		if (bunch == 994)
			return 1;
		if (bunch == 991)
			return 2;
	}
	
	if (run >= 8333 && run <= 8341)
	{
		if (bunch == 900)
			return 1;
		if (bunch == 991)
			return 2;
	}

	if (run >= 8367 && run <= 8372)
	{
		if (bunch == 3104 || bunch == 3130 || bunch == 3156 || bunch == 3078)
			return 1;
		if (bunch == 3143 || bunch == 3169 || bunch == 3195 || bunch == 3117)
			return 2;
	}
	*/

	return 0;
}

//----------------------------------------------------------------------------------------------------

bool IsZeroBias(unsigned int trigger, unsigned int /*run*/, unsigned int /*event*/)
{
	return ((trigger & 512) != 0);
}

//----------------------------------------------------------------------------------------------------

HitData ProtonTransport(const Kinematics &k, const Environment &env)
{
	HitData h;

	h.L_2_F.x = -env.L_x_L_2_F*k.th_x_L + env.v_x_L_2_F*k.vtx_x; //   - env.la_x_L_2_F*k.th_y_L;
	h.L_2_F.y = -env.L_y_L_2_F*k.th_y_L + env.v_y_L_2_F*k.vtx_y; //   - env.la_y_L_2_F*k.th_x_L;

	h.L_1_F.x = -env.L_x_L_1_F*k.th_x_L + env.v_x_L_1_F*k.vtx_x; //   - env.la_x_L_1_F*k.th_y_L;
	h.L_1_F.y = -env.L_y_L_1_F*k.th_y_L + env.v_y_L_1_F*k.vtx_y; //   - env.la_y_L_1_F*k.th_x_L;

	h.R_1_F.x = +env.L_x_R_1_F*k.th_x_R + env.v_x_R_1_F*k.vtx_x; //   + env.la_x_R_1_F*k.th_y_R;
	h.R_1_F.y = +env.L_y_R_1_F*k.th_y_R + env.v_y_R_1_F*k.vtx_y; //   + env.la_y_R_1_F*k.th_x_R;

	h.R_2_F.x = +env.L_x_R_2_F*k.th_x_R + env.v_x_R_2_F*k.vtx_x; //   + env.la_x_R_2_F*k.th_y_R;
	h.R_2_F.y = +env.L_y_R_2_F*k.th_y_R + env.v_y_R_2_F*k.vtx_y; //   + env.la_y_R_2_F*k.th_x_R;

	return h;
}

//----------------------------------------------------------------------------------------------------

double *th_x_binning_edges_1d=NULL, *th_y_binning_edges_1d=NULL;
double *th_x_binning_edges_2d=NULL, *th_y_binning_edges_2d=NULL;
double *th_x_binning_edges_2d_coarse=NULL, *th_y_binning_edges_2d_coarse=NULL;

int th_x_binning_n_1d=0, th_y_binning_n_1d=0;
int th_x_binning_n_2d=0, th_y_binning_n_2d=0;
int th_x_binning_n_2d_coarse=0, th_y_binning_n_2d_coarse=0;

void BuildThBinning()
{
	deque<double> edges;

	// ----- th_x, 1D -----
	edges.clear();
	for (double x = 0E-6; ; )
	{
		edges.push_back(x);
		if (x != 0E-6)
			edges.push_front(-x);

		if (x > 180E-6)
			break;

		x += 5.000000E-06 + -1.071429E-02 * x + 3.214286E+03 * x*x;
	}

	th_x_binning_edges_1d = new double[edges.size()];
	th_x_binning_n_1d = edges.size() - 1;
	for (unsigned int i = 0; i < edges.size(); i++)
		th_x_binning_edges_1d[i] = edges[i];

	// ----- th_y, 1D -----
	edges.clear();
	for (double y = 0E-6; ; )
	{
		edges.push_back(y);
		if (y > 120E-6)
			break;
		y += 2.000000E-06 + 4.166667E-03 * y + 4.427083E+07 * y*y*y;
	}

	th_y_binning_edges_1d = new double[edges.size()];
	th_y_binning_n_1d = edges.size() - 1;
	for (unsigned int i = 0; i < edges.size(); i++)
		th_y_binning_edges_1d[i] = edges[i];

	// ----- th_x, 2D -----
	edges.clear();
	for (double x = 0E-6; ; )
	{
		edges.push_back(x);
		if (x != 0E-6)
			edges.push_front(-x);

		if (x > 140E-6)
			break;

		//x += 5.000000E-06 + -1.071429E-02 * x + 3.214286E+03 * x*x;
		//x += 5.000000E-06 + -4.761905E-04 * x + 3.476190E+03 * x*x;
		x += 5.000000E-06 + 5.155844E-02 * x + 2.467532E+03 * x*x;
	}

	th_x_binning_edges_2d = new double[edges.size()];
	th_x_binning_n_2d = edges.size() - 1;
	for (unsigned int i = 0; i < edges.size(); i++)
	{
		th_x_binning_edges_2d[i] = edges[i];
		printf("  %i --> edge x = %E\n", i, edges[i]);
	}

	// ----- th_y, 2D -----
	edges.clear();
	for (double y = 0E-6; ; )
	{
		edges.push_back(y);
		if (y > 100E-6)
			break;

		//y += 2.000000E-06 + 4.166667E-03 * y + 4.427083E+07 * y*y*y;
		y += 2.000000E-06 + 2.083333E-02 * y + 4.166667E+07 * y*y*y;
	}

	printf("\n");

	th_y_binning_edges_2d = new double[edges.size()];
	th_y_binning_n_2d = edges.size() - 1;
	for (unsigned int i = 0; i < edges.size(); i++)
	{
		th_y_binning_edges_2d[i] = edges[i];
		printf("  %i --> edge y = %E\n", i, edges[i]);
	}

	// ----- th_x, 2D, coarse -----
	edges = { -200E-6, -150E-6, -100E-6, 100E-6, 150E-6, 200E-6 };

	th_x_binning_edges_2d_coarse = new double[edges.size()];
	th_x_binning_n_2d_coarse = edges.size() - 1;
	for (unsigned int i = 0; i < edges.size(); i++)
		th_x_binning_edges_2d_coarse[i] = edges[i];

	// ----- th_y, 2D, coarse -----
	edges = { 30E-6, 50E-6, 70E-6, 100E-6};

	th_y_binning_edges_2d_coarse = new double[edges.size()];
	th_y_binning_n_2d_coarse = edges.size() - 1;
	for (unsigned int i = 0; i < edges.size(); i++)
		th_y_binning_edges_2d_coarse[i] = edges[i];

	printf(">> BuildThBinning\n");
	printf("\n1D, th_x: %i, %p\n", th_x_binning_n_1d, th_x_binning_edges_1d);
	printf("\n1D, th_y: %i, %p\n", th_y_binning_n_1d, th_y_binning_edges_1d);
	printf("\n2D, th_x: %i, %p\n", th_x_binning_n_2d, th_x_binning_edges_2d);
	printf("\n2D, th_y: %i, %p\n", th_y_binning_n_2d, th_y_binning_edges_2d);
	printf("\n2D coarse, th_x: %i, %p\n", th_x_binning_n_2d_coarse, th_x_binning_edges_2d_coarse);
	printf("\n2D coarse, th_y: %i, %p\n", th_y_binning_n_2d_coarse, th_y_binning_edges_2d_coarse);
}

#endif
