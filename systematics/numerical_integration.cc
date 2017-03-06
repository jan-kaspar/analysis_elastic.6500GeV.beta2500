#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TF1.h"

#include <string>
#include <vector>
#include <set>

#include "common.h"

#include "../NumericalIntegration.h"
#include "../common_definitions.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

double p = 6.5E3;	// GeV

// fiducial acceptance cuts
FiducialCut fc_G_l;
FiducialCut fc_G_h;

//----------------------------------------------------------------------------------------------------

// reconstruction shifts (rad)
double sh_th_x = 0., sh_th_y = 0.;

// reconstruction tilt (rad)
double thx_thy_tilt;

// reconstruction scales, th_y_reco = sc_th_y * th_y_true
double sc_th_x = 1., sc_th_y = 1.;

// relative error of beam momentum
double de_rel_p;
		
// uncertainty of the 3-out-of-4 efficiency plateau slope
double eff_slope_error;
double eff_slope_fix_point;

double de_norm;

//----------------------------------------------------------------------------------------------------

int SetScenario(const string &scenario)
{
	// defaults (no systematics)
	sh_th_x = sh_th_y = 0E-6;
	sc_th_x = sc_th_y = 1.;

	thx_thy_tilt = 0.;

	de_rel_p = 0.;

	eff_slope_error = 0.;	
	eff_slope_fix_point = 0E-6;

	de_norm = 0.;

	if (scenario.compare("none") == 0)
	{
		return 0;
	}

	if (scenario.compare("alig-sh-thx") == 0)
	{
		sh_th_x = 1E-6;
		return 0;
	}

	if (scenario.compare("alig-sh-thy") == 0)
	{
		sh_th_y = 0.24E-6;
		return 0;
	}

	if (scenario.compare("thx-thy-tilt") == 0)
	{
		thx_thy_tilt = 0.005;
		return 0;
	}

	if (scenario.compare("opt-m1") == 0)
	{
		sc_th_x = 1. - 1E-03;
		sc_th_y = 1.;

		return 0;
	}

	if (scenario.compare("opt-m2") == 0)
	{
		sc_th_x = 1.;
		sc_th_y = 1. - 1E-03;

		return 0;
	}

	// TODO
	/*
	if (scenario.compare("eff-slp") == 0)
	{
		eff_slope_fix_point = 40E-6;		// |th_y|, rad
		if (ds == ds2a)
			eff_slope_error = 90.;			// 1/rad
		if (ds == ds2b)
			eff_slope_error = 60.;			// 1/rad

		return 0;
	}
	*/

	if (scenario.compare("beam-mom") == 0)
	{
		de_rel_p = -0.001;

		return 0;
	}

	if (scenario.compare("norm") == 0)
	{
		de_norm = 0.10;

		return 0;
	}

	printf("ERROR: unknown scenario `%s'.\n", scenario.c_str());
	return 1;
}

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

	fc_G_l = FiducialCut(4.3E-6, -20E-6, -0.01, +10E-6, +0.05);
	fc_G_h = FiducialCut(100E-6, 0E-6, 0., 0E-6, 0.);

	return 0;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

unsigned long int_ws_phi_size;
gsl_integration_workspace *int_ws_phi;

//----------------------------------------------------------------------------------------------------

TSpline *dist_true_t_spline = NULL;

double dist_true_t(double t)
{
	return dist_true_t_spline->Eval(t);
}

//----------------------------------------------------------------------------------------------------

double dist_true_th_x_th_y(double th_x, double th_y)
{
	double t = (th_x*th_x + th_y*th_y) * p*p;
	return dist_true_t(t);
}

//----------------------------------------------------------------------------------------------------

bool use_reco_dist_th_x_th_y;

double dist_reco_th_x_th_y(double th_x_p, double th_y_p)
{
	double eff_slp_corr = eff_slope_error * (fabs(th_y_p) - eff_slope_fix_point);

	double de_p_norm_corr = (use_reco_dist_th_x_th_y) ? (1. + de_rel_p) : 1.;

	// transformation from reconstructed (primed) to original/true (non-primed) angles
	// Th' = M Th + De Th  ==>  Th = M^-1 (Th' - De The)
	// Mi = M^-1
	double th_x_ps = th_x_p - sh_th_x;
	double th_y_ps = th_y_p - sh_th_y;

	double Mi_xx = 1. / sc_th_x, Mi_xy = -thx_thy_tilt;
	double Mi_yx = 0., Mi_yy = 1. / sc_th_y;

	double D = Mi_xx * Mi_yy - Mi_xy * Mi_yx;

	double th_x = Mi_xx * th_x_ps + Mi_xy * th_y_ps;
	double th_y = Mi_yx * th_x_ps + Mi_yy * th_y_ps;

	return dist_true_th_x_th_y(th_x, th_y)
		* D
		* (1. + eff_slp_corr)
		* (1. + de_norm)
		/ de_p_norm_corr / de_p_norm_corr;
}

//----------------------------------------------------------------------------------------------------

double IntegOverPhi(double x, double *par, const void *)
{
	double phi = x;
	double th_p = par[0];

	double th_x_p = th_p * cos(phi);
	double th_y_p = th_p * sin(phi);

	double result = (use_reco_dist_th_x_th_y) ? dist_reco_th_x_th_y(th_x_p, th_y_p) : dist_true_th_x_th_y(th_x_p, th_y_p);
	//printf("phi = %.3f => %.5E\n", phi, result);

	return result;
}

//----------------------------------------------------------------------------------------------------

bool applyAcceptanceCorrection = false;

double dist_reco_t(double t_p)
{
	double p_p = (use_reco_dist_th_x_th_y) ? p*(1. + de_rel_p) : p;
	double th_p = sqrt(t_p) / p_p;
	
	double param[1] = { th_p };

	// get all intersections of const-th circle with acceptance boundaries
	set<double> phis;

	for (const auto &phi : fc_G_l.GetIntersectionPhis(th_p))
		phis.insert(phi);

	for (const auto &phi : fc_G_h.GetIntersectionPhis(th_p))
		phis.insert(phi);

	// the number of intersections must be even
	if ((phis.size() % 2) == 1)
	{
		printf("ERROR: odd number of intersections in acceptance calculation\n");
	}

	// no intersection => no acceptances
	if (phis.size() == 0)
		return 0.;

	// calculate integrals over phi sections
	double precision = 1E-5;
	double phiSum = 0.;
	double integralSum = 0.;
	for (set<double>::iterator it = phis.begin(); it != phis.end(); ++it)
	{
		double phi_start = *it;
		++it;
		double phi_end = *it;

		phiSum += phi_end - phi_start;

		if (th_y_sign == +1)
			integralSum += RealIntegrate(IntegOverPhi, param, NULL, phi_start, phi_end, 0., precision, int_ws_phi_size, int_ws_phi, "dist_reco_t");
		else
			integralSum += RealIntegrate(IntegOverPhi, param, NULL, -phi_end, -phi_start, 0., precision, int_ws_phi_size, int_ws_phi, "dist_reco_t");
	}

	if (applyAcceptanceCorrection)
		return integralSum / phiSum;
	else
		return integralSum;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: numerical_integration <dataset> <diagonal> <output file name>\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	if (argc != 4)
	{
		printf("ERROR: wrong number of parameters\n");
		PrintUsage();
		return 1;
	}

	// get command-line arguments
	string dataset = argv[1];
	string diagonal = argv[2];
	string outFileName = argv[3];

	// set-up for chosen dataset and diagonal
	if (Setup(dataset, diagonal) != 0)
	{
		printf("ERROR: Setup failed.\n");
		return 2;
	}

	// scenarios
	vector<string> scenarios;
	scenarios.push_back("none");
	scenarios.push_back("alig-sh-thx");
	scenarios.push_back("alig-sh-thy");
	scenarios.push_back("thx-thy-tilt");
	scenarios.push_back("opt-m1");
	scenarios.push_back("opt-m2");
	// TODO
	//scenarios.push_back("eff-slp");
	scenarios.push_back("beam-mom");
	scenarios.push_back("beam-mom-alfa");
	scenarios.push_back("norm");

	// load input dsigma/dt distribution
	if (LoadTDistributions() != 0)
		return 1;

	// prepare output
	TFile *f_out = new TFile(outFileName.c_str(), "recreate");

	// initialise integration workspaces
	int_ws_phi_size = 100;
	int_ws_phi = gsl_integration_workspace_alloc(int_ws_phi_size);

	// evaluate effects
	for (const auto &inputDist : inputTDistributions)
	{
		printf("    input distribution: %s\n", inputDist.label.c_str());
		TDirectory *d_dist = f_out->mkdir(inputDist.label.c_str());
		gDirectory = d_dist;

		inputDist.g_dsdt->Write("g_dsdt");
		inputDist.s_dsdt->Write("s_dsdt");

		dist_true_t_spline = inputDist.s_dsdt;

		for (unsigned int sci = 0; sci < scenarios.size(); sci++)
		{
			const string &scenario = scenarios[sci];

			if (SetScenario(scenario) != 0)
				continue;
	
			printf("        scenario: %s\n", scenario.c_str());
			TDirectory *d_scenario = d_dist->mkdir(scenario.c_str());
			gDirectory = d_scenario;

			// sample distributions
			TGraph *g_dsdt_true = new TGraph(); g_dsdt_true->SetName("g_dsdt_true"); g_dsdt_true->SetLineColor(1);
			TGraph *g_dsdt_idre = new TGraph(); g_dsdt_idre->SetName("g_dsdt_idre"); g_dsdt_idre->SetLineColor(2);
			TGraph *g_dsdt_reco = new TGraph(); g_dsdt_reco->SetName("g_dsdt_reco"); g_dsdt_reco->SetLineColor(4);
			TGraph *g_eff = new TGraph(); g_eff->SetName("g_eff"); g_eff->SetLineColor(8);

			for (double t = 8E-4; t <= 0.3;)
			{
				double v_true = dist_true_t(t);

				applyAcceptanceCorrection = true;
				use_reco_dist_th_x_th_y = false;
				double v_idre = dist_reco_t(t);

				applyAcceptanceCorrection = true;
				use_reco_dist_th_x_th_y = true;
				double v_reco = dist_reco_t(t);

				double v_eff = (v_idre != 0.) ? v_reco / v_idre : 0.;

				int idx = g_dsdt_true->GetN();

				g_dsdt_true->SetPoint(idx, t, v_true);
				g_dsdt_idre->SetPoint(idx, t, v_idre);
				g_dsdt_reco->SetPoint(idx, t, v_reco);
				g_eff->SetPoint(idx, t, v_eff);

				// advance t
				double dt = 0.001;
				if (t < 0.01) dt = 0.001;
				if (t < 4E-3) dt = 0.0001;
				if (t < 1E-3) dt = 0.00005;
				t += dt;
			}

			g_dsdt_true->Write();
			g_dsdt_idre->Write();
			g_dsdt_reco->Write();
			g_eff->Write();
		}
	}

	delete f_out;

	return 0;
}
