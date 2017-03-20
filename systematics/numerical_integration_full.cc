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
#include "../AcceptanceCalculator.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

double p = 6500.;	// GeV

Analysis anal;

//----------------------------------------------------------------------------------------------------

double si_d_x, si_d_y, si_m_x, si_m_y;

double range_m_x, range_m_y, range_d_x, range_d_y;

//----------------------------------------------------------------------------------------------------

bool apply_reconstruction_bias;

//----------------------------------------------------------------------------------------------------

AcceptanceCalculator acceptanceCalculator;

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

	// TODO: preliminary - final should be diagonal dependent
	anal.fc_L_l = FiducialCut(4.1E-6, -20E-6, -0.01, +10E-6, +0.05);
	anal.fc_L_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_R_l = FiducialCut(4.2E-6, -20E-6, -0.01, +20E-6, 0.05);
	anal.fc_R_h = FiducialCut(102E-6, 0E-6, 0., 0E-6, 0.);

	anal.fc_G_l = FiducialCut(4.3E-6, -20E-6, -0.01, +10E-6, +0.05);
	anal.fc_G_h = FiducialCut(100E-6, 0E-6, 0., 0E-6, 0.);

	//--------------------------------------------------

	anal.si_th_x_LRdiff = 1.250000E-05; 
	anal.si_th_y_1arm = 2.616295E-07;

	anal.si_th_x_2arm = 3.000000E-07;
	anal.si_th_y_2arm = 1.850000E-07;

	si_d_x = anal.si_th_x_LRdiff;
	si_d_y = anal.si_th_y_1arm * sqrt(2.);
	si_m_x = anal.si_th_x_2arm;
    si_m_y = anal.si_th_y_2arm;

	range_d_x = 5. * si_d_x;
	range_d_y = 5. * si_d_y;
	range_m_x = 5. * si_m_x;
	range_m_y = 5. * si_m_y;

	//--------------------------------------------------

	acceptanceCalculator.Init(th_y_sign, anal);

	return 0;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

// reconstruction shifts (rad)
double sh_th_x = 0., sh_th_y = 0.;

// reconstruction tilt (rad)
double thx_thy_tilt;

// reconstruction scales, th_y_reco = sc_th_y * th_y_true
double sc_th_x = 1., sc_th_y = 1.;

// relative error of beam momentum
double de_rel_p;
		
// errors of the 3-out-of-4 efficiency plateau slope
double eff_slope_error;
double eff_slope_fix_point;

// normalisation error
double de_norm;

// errors of unsmearing parameters
double si_d_x_err;
double si_d_y_err;

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

	si_d_x_err = 0.;
	si_d_y_err = 0.;

	// scenario definitions

	if (scenario.compare("none") == 0)
	{
		return 0;
	}

	if (scenario.compare("alig-sh-thx") == 0)
	{
		sh_th_x = 0.5E-6;
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
		// TODO: preliminary
		sc_th_x = 1. - 1E-03;
		sc_th_y = 1.;

		return 0;
	}

	if (scenario.compare("opt-m2") == 0)
	{
		// TODO: preliminary
		sc_th_x = 1.;
		sc_th_y = 1. - 1E-03;

		return 0;
	}

	if (scenario.compare("eff-slp") == 0)
	{
		// TODO: preliminary
		eff_slope_fix_point = 30E-6;	// |th_y|, rad
		eff_slope_error = 15.;			// 1/rad

		return 0;
	}

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

	if (scenario.compare("acc-corr-sigma-unc-thx") == 0)
	{
		si_d_x_err = 0.3E-6;

		return 0;
	}

	if (scenario.compare("acc-corr-sigma-unc-thy") == 0)
	{
		si_d_y_err = 0.007E-6;

		return 0;
	}

	printf("ERROR: unknown scenario `%s'.\n", scenario.c_str());
	return 1;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

unsigned long int_ws_phi_size;
gsl_integration_workspace *int_ws_phi;

unsigned long int_ws_m_x_size;
gsl_integration_workspace *int_ws_m_x;

unsigned long int_ws_m_y_size;
gsl_integration_workspace *int_ws_m_y;

unsigned long int_ws_d_x_size;
gsl_integration_workspace *int_ws_d_x;

unsigned long int_ws_d_y_size;
gsl_integration_workspace *int_ws_d_y;

//----------------------------------------------------------------------------------------------------

double dist_m_x(double m_x)
{
	const double r = m_x / si_m_x;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_m_x;
}

//----------------------------------------------------------------------------------------------------

double dist_m_y(double m_y)
{
	const double r = m_y / si_m_y;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_m_y;
}

//----------------------------------------------------------------------------------------------------

double dist_d_x(double d_x)
{
	double si_d_x_eff = si_d_x;
	if (apply_reconstruction_bias)
		si_d_x_eff += si_d_x_err;

	const double r = d_x / si_d_x_eff;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_d_x_eff;
}

//----------------------------------------------------------------------------------------------------

double dist_d_y(double d_y)
{
	double si_d_y_eff = si_d_y;
	if (apply_reconstruction_bias)
		si_d_y_eff += si_d_y_err;

	const double r = d_y / si_d_y_eff;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_d_y_eff;
}

//----------------------------------------------------------------------------------------------------

TSpline *dist_true_t_spline = NULL;

double dist_t_true(double t)
{
	return dist_true_t_spline->Eval(t);
}

//----------------------------------------------------------------------------------------------------

double dist_th_x_th_y_true(double th_x, double th_y)
{
	double t = (th_x*th_x + th_y*th_y) * p*p;
	return dist_t_true(t);
}

//----------------------------------------------------------------------------------------------------

double dist_th_x_th_y_syst(double th_x_p, double th_y_p)
{
	if (!apply_reconstruction_bias)
		return dist_th_x_th_y_true(th_x_p, th_y_p);

	double eff_slp_corr = eff_slope_error * (fabs(th_y_p) - eff_slope_fix_point);

	double de_p_norm_corr = 1. + de_rel_p;

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

	return dist_th_x_th_y_true(th_x, th_y)
		* D
		* (1. + eff_slp_corr)
		* (1. + de_norm)
		/ de_p_norm_corr / de_p_norm_corr;
}

//----------------------------------------------------------------------------------------------------

double IntegOverMY(double m_y, double *param, const void *)
{
	const double &th_x_p = param[0];
	const double &th_y_p = param[1];
	const double &m_x = param[2];

	const double th_x = th_x_p - m_x;
	const double th_y = th_y_p - m_y;

	return dist_th_x_th_y_syst(th_x, th_y) * dist_m_y(m_y);
}

//----------------------------------------------------------------------------------------------------

double IntegOverMX(double m_x, double *param, const void *)
{
	const double &th_x_p = param[0];
	const double &th_y_p = param[1];

	double param_out[] = { th_x_p, th_y_p, m_x };
	
	const double rel_precision = 1E-3;

	const double I = RealIntegrate(IntegOverMY, param_out, NULL, -range_m_y, +range_m_y, 0., rel_precision, int_ws_m_y_size, int_ws_m_y, "IntegOverMX");

	return I * dist_m_x(m_x);
}

//----------------------------------------------------------------------------------------------------

double dist_th_x_th_y_smea(double th_x_p, double th_y_p)
{
	double param[] = { th_x_p, th_y_p };

	const double rel_precision = 1E-3;

	return RealIntegrate(IntegOverMX, param, NULL, -range_m_x, +range_m_x, 0., rel_precision, int_ws_m_x_size, int_ws_m_x, "dist_th_x_th_y_smea");
}

//----------------------------------------------------------------------------------------------------

double IntegOverDY(double d_y, double *, const void *)
{
	return dist_d_y(d_y);
}

//----------------------------------------------------------------------------------------------------

double IntegOverDX(double d_x, double *param, const void *)
{
	const double &th_x_p = param[0];
	const double &th_y_p = param[1];

	const double th_x_p_L = th_x_p - d_x/2.;
	const double th_x_p_R = th_x_p + d_x/2.;

	double th_y_L_cut_l = anal.fc_L_l.GetThYLimit(th_x_p_L);
	double th_y_L_cut_h = anal.fc_L_h.GetThYLimit(th_x_p_L);

	double th_y_R_cut_l = anal.fc_R_l.GetThYLimit(th_x_p_R);
	double th_y_R_cut_h = anal.fc_R_h.GetThYLimit(th_x_p_R);

	// TODO: is this correct?
	double th_y_abs = th_y_sign * th_y_p;
	double d_y_min = 2. * max( th_y_R_cut_l - th_y_abs, th_y_abs - th_y_L_cut_h );
	double d_y_max = 2. * min( th_y_R_cut_h - th_y_abs, th_y_abs - th_y_L_cut_l );

	if (d_y_min >= d_y_max)
		return 0;
	
	const double rel_precision = 1E-3;

	const double I = RealIntegrate(IntegOverDY, NULL, NULL, d_y_min, d_y_max, 0., rel_precision, int_ws_d_y_size, int_ws_m_y, "IntegOverDX");

	return I * dist_d_x(d_x);
}

//----------------------------------------------------------------------------------------------------

double acceptance_smea(double th_x_p, double th_y_p)
{
	double param[] = { th_x_p, th_y_p };
	
	const double rel_precision = 1E-3;

	return RealIntegrate(IntegOverDX, param, NULL, -range_d_x, +range_d_x, 0., rel_precision, int_ws_d_x_size, int_ws_d_x, "acceptance_smea");
}

//----------------------------------------------------------------------------------------------------

double dist_reco_th_x_th_y(double th_x_p, double th_y_p)
{
	// evaluate smearing acceptance correction as in the analysis
	double F = acceptanceCalculator.SmearingFactor(th_x_p, th_y_p);
	double corr_acc_sm = 1. / F;

	return dist_th_x_th_y_smea(th_x_p, th_y_p) * acceptance_smea(th_x_p, th_y_p) * corr_acc_sm;
}

//----------------------------------------------------------------------------------------------------

double IntegOverPhi(double phi, double *par, const void *)
{
	const double th_p = par[0];

	const double th_x_p = th_p * cos(phi);
	const double th_y_p = th_p * sin(phi);

	double result = dist_reco_th_x_th_y(th_x_p, th_y_p);

	return result;
}

//----------------------------------------------------------------------------------------------------

double dist_t_reco(double t_p)
{
	const double p_p = (apply_reconstruction_bias) ? p*(1. + de_rel_p) : p;
	const double th_p = sqrt(t_p) / p_p;
	
	double param[1] = { th_p };

	// get all intersections of const-th circle with acceptance boundaries
	set<double> phis;

	for (const auto &phi : anal.fc_G_l.GetIntersectionPhis(th_p))
		phis.insert(phi);

	for (const auto &phi : anal.fc_G_h.GetIntersectionPhis(th_p))
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
	const double rel_precision = 1E-3;
	double phiSum = 0.;
	double integralSum = 0.;
	for (set<double>::iterator it = phis.begin(); it != phis.end(); ++it)
	{
		double phi_start = *it;
		++it;
		double phi_end = *it;

		phiSum += phi_end - phi_start;

		if (th_y_sign == +1)
			integralSum += RealIntegrate(IntegOverPhi, param, NULL, phi_start, phi_end, 0., rel_precision, int_ws_phi_size, int_ws_phi, "dist_reco_t");
		else
			integralSum += RealIntegrate(IntegOverPhi, param, NULL, -phi_end, -phi_start, 0., rel_precision, int_ws_phi_size, int_ws_phi, "dist_reco_t");
	}

	return integralSum / phiSum;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: numerical_integration_full <dataset> <diagonal> <output file name>\n");
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
	scenarios.push_back("eff-slp");
	scenarios.push_back("beam-mom");
	scenarios.push_back("norm");
	scenarios.push_back("acc-corr-sigma-unc-thx");
	scenarios.push_back("acc-corr-sigma-unc-thy");

	// load input dsigma/dt distribution
	if (LoadTDistributions() != 0)
		return 1;

	// prepare output
	TFile *f_out = new TFile(outFileName.c_str(), "recreate");

	// initialise integration workspaces
	int_ws_phi_size = 100;
	int_ws_phi = gsl_integration_workspace_alloc(int_ws_phi_size);

	int_ws_m_x_size = 100;
	int_ws_m_x = gsl_integration_workspace_alloc(int_ws_m_x_size);

	int_ws_m_y_size = 100;
	int_ws_m_y = gsl_integration_workspace_alloc(int_ws_m_y_size);

	int_ws_d_x_size = 100;
	int_ws_d_x = gsl_integration_workspace_alloc(int_ws_d_x_size);

	int_ws_d_y_size = 100;
	int_ws_d_y = gsl_integration_workspace_alloc(int_ws_d_y_size);

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
				//printf("            t = %.2E\n", t);

				double v_true = dist_t_true(t);

				apply_reconstruction_bias = false;
				double v_idre = dist_t_reco(t);

				apply_reconstruction_bias = true;
				double v_reco = dist_t_reco(t);

				double v_eff = (v_idre != 0.) ? v_reco / v_idre : 0.;

				int idx = g_dsdt_true->GetN();

				g_dsdt_true->SetPoint(idx, t, v_true);
				g_dsdt_idre->SetPoint(idx, t, v_idre);
				g_dsdt_reco->SetPoint(idx, t, v_reco);
				g_eff->SetPoint(idx, t, v_eff);

				// advance t
				double dt = 0.005;
				if (t < 0.02) dt = 0.001;
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
