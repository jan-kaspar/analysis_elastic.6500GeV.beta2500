#include "TFile.h"
#include "TGraph.h"
#include "TMath.h"

#include "../common_definitions.h"
#include "../common_algorithms.h"
#include "../AcceptanceCalculator.h"

#include "parameters.h"
#include "../common.h"

#include "scenarios.h"
#include "input_distributions.h"

#include "command_line_tools.h"

#include <vector>
#include <string>


using namespace std;

//----------------------------------------------------------------------------------------------------

Environment env_sim, env_rec;
Analysis anal_sim, anal_rec;

double ineff_intercept, ineff_slope;

Biases biases;

AcceptanceCalculator accCalc;

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
	const double &si_m_x = anal_sim.si_th_x_2arm;
	const double r = m_x / si_m_x;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_m_x;
}

//----------------------------------------------------------------------------------------------------

double dist_m_y(double m_y)
{
	const double &si_m_y = anal_sim.si_th_y_2arm;
	const double r = m_y / si_m_y;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_m_y;
}

//----------------------------------------------------------------------------------------------------

double dist_d_x(double d_x)
{
	if (biases.use_non_gaussian_d_x)
	{
		return NonGauassianDistribution_d_x(d_x);
	} else {
		const double &si_d_x = anal_sim.si_th_x_LRdiff;
		const double r = d_x / si_d_x;
		return exp(-r*r/2.) / sqrt(2. * M_PI) / si_d_x;
	}
}

//----------------------------------------------------------------------------------------------------

double dist_d_y(double d_y)
{
	const double &si_d_y = anal_sim.si_th_y_LRdiff;
	const double r = d_y / si_d_y;
	return exp(-r*r/2.) / sqrt(2. * M_PI) / si_d_y;
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
	const double &p = env_sim.p;
	double t = (th_x*th_x + th_y*th_y) * p*p;
	return dist_t_true(t);
}

//----------------------------------------------------------------------------------------------------

double dist_th_x_th_y_syst(double th_x_p, double th_y_p)
{
	// error in inefficiency corrections
	const double ineff_simu = ineff_intercept + ineff_slope * fabs(th_y_p);
	const double ineff_reco = (ineff_intercept + biases.eff_intercept) + (ineff_slope + biases.eff_slope) * fabs(th_y_p);

	const double eff_corr_effect = (1. - ineff_simu) / (1. - ineff_reco);

	// L-R average of biases
	const double mbias_sh_th_x = (biases.L.sh_th_x + biases.R.sh_th_x) / 2.;
	const double mbias_sh_th_y = (biases.L.sh_th_y + biases.R.sh_th_y) / 2.;

	const double mbias_sc_th_x = (biases.L.sc_th_x + biases.R.sc_th_x) / 2.;
	const double mbias_sc_th_y = (biases.L.sc_th_y + biases.R.sc_th_y) / 2.;

	const double mbias_tilt_xy = (biases.L.tilt_th_x_eff_prop_to_th_y + biases.R.tilt_th_x_eff_prop_to_th_y) / 2.;
	const double mbias_tilt_yx = (biases.L.tilt_th_y_eff_prop_to_th_x + biases.R.tilt_th_y_eff_prop_to_th_x) / 2.;

	// transformation from reconstructed (primed) to original/true (non-primed) angles
	// Th' = M Th + De Th  ==>  Th = M^-1 (Th' - De The)
	// Mi = M^-1

	const double th_x_ps = th_x_p - mbias_sh_th_x;
	const double th_y_ps = th_y_p - mbias_sh_th_y;

	const double Mi_xx = 1. / mbias_sc_th_x, Mi_xy = -mbias_tilt_xy;
	const double Mi_yx = -mbias_tilt_yx, Mi_yy = 1. / mbias_sc_th_y;

	const double D = Mi_xx * Mi_yy - Mi_xy * Mi_yx;

	const double th_x = Mi_xx * th_x_ps + Mi_xy * th_y_ps;
	const double th_y = Mi_yx * th_x_ps + Mi_yy * th_y_ps;

	return dist_th_x_th_y_true(th_x, th_y)
		* D
		* eff_corr_effect
		* (1. + biases.norm);
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
	const double range_m_y = 5. * anal_sim.si_th_y_2arm; 

	const double I = RealIntegrate(IntegOverMY, param_out, NULL, -range_m_y, +range_m_y, 0., rel_precision, int_ws_m_y_size, int_ws_m_y, "IntegOverMX");

	return I * dist_m_x(m_x);
}

//----------------------------------------------------------------------------------------------------

double dist_th_x_th_y_smea(double th_x_p, double th_y_p)
{
	double param[] = { th_x_p, th_y_p };

	const double rel_precision = 1E-3;
	const double range_m_x = 5. * anal_sim.si_th_x_2arm; 

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

	// extract anti-symmetric component of the shift bias
	const double abias_sh_th_x = (biases.R.sh_th_x - biases.L.sh_th_x) / 2.;
	const double abias_sh_th_y = (biases.R.sh_th_y - biases.L.sh_th_y) / 2.;

	const double th_x_p_L = th_x_p - d_x/2. - abias_sh_th_x;
	const double th_x_p_R = th_x_p + d_x/2. + abias_sh_th_x;

	double th_y_L_cut_l = anal_sim.fc_L_l.GetThYLimit(th_x_p_L);
	double th_y_L_cut_h = anal_sim.fc_L_h.GetThYLimit(th_x_p_L);

	double th_y_R_cut_l = anal_sim.fc_R_l.GetThYLimit(th_x_p_R);
	double th_y_R_cut_h = anal_sim.fc_R_h.GetThYLimit(th_x_p_R);

	double th_y_abs = th_y_sign * th_y_p;
	double d_y_min = 2. * max( th_y_R_cut_l - th_y_abs, th_y_abs - th_y_L_cut_h );
	double d_y_max = 2. * min( th_y_R_cut_h - th_y_abs, th_y_abs - th_y_L_cut_l );

	if (d_y_min >= d_y_max)
		return 0;

	// apply asymmetric bias in th_y
	d_y_min -= 2. * abias_sh_th_y;
	d_y_max -= 2. * abias_sh_th_y;

	const bool gaussianOptimisation = false;
	const double rel_precision = 1E-4;

	double I;
	if (gaussianOptimisation)
	{
		const double si_d_y = anal_sim.si_th_y_LRdiff;
		I = ( TMath::Erf(d_y_max / sqrt(2.) / si_d_y) - TMath::Erf(d_y_min / sqrt(2.) / si_d_y) ) / 2.;
	} else {
		I = RealIntegrate(IntegOverDY, NULL, NULL, d_y_min, d_y_max, 0., rel_precision, int_ws_d_y_size, int_ws_d_y, "IntegOverDX");
	}

	return I * dist_d_x(d_x);
}

//----------------------------------------------------------------------------------------------------

double acceptance_smea(double th_x_p, double th_y_p)
{
	double param[] = { th_x_p, th_y_p };

	const double abs_precision = 1E-3;
	const double range_d_x = 5. * anal_sim.si_th_x_LRdiff;

	return RealIntegrate(IntegOverDX, param, NULL, -range_d_x, +range_d_x, abs_precision, 0., int_ws_d_x_size, int_ws_d_x, "acceptance_smea");
}

//----------------------------------------------------------------------------------------------------

double dist_reco_th_x_th_y(double th_x_p, double th_y_p)
{
	// evaluate smearing acceptance correction as in the analysis
	double F = accCalc.SmearingFactor(th_x_p, th_y_p);
	double corr_acc_sm = (F == 0.) ? 0. : 1. / F;

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
	const double &p_p = env_rec.p;
	const double th_p = sqrt(t_p) / p_p;

	double param[1] = { th_p };

	// get all intersections of const-th circle with acceptance boundaries
	set<double> phis;

	for (const auto &phi : anal_rec.fc_G_l.GetIntersectionPhis(th_p))
		phis.insert(phi);

	for (const auto &phi : anal_rec.fc_G_h.GetIntersectionPhis(th_p))
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
	printf("USAGE: ni_simu [option] [option] ...\n");
	printf("OPTIONS:\n");
	printf("    -diagonal <str>\n");
	printf("    -scenario <str>     systematic scenario\n");
	printf("    -model <str>        dsigma/dt model\n");
	printf("    -output <file>\n");

}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// defaults
	string diagonalStr = "";
	string scenario = "none";
	string model = "fit2-2";

	string outFileName = "ouput.root";

	ineff_intercept = 0.067 + 0.03; // sum from 3/4 and 2/4, respectively
	ineff_slope = 140.;	// rad^-1

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
	printf("    ineff_intercept = %.3E\n", ineff_intercept);
	printf("    ineff_slope = %.3E\n", ineff_slope);
	printf("    outFileName = %s\n", outFileName.c_str());

	// init diagonal settings
	Init(diagonalStr);

	if (diagonal == dUnknown)
	{
		printf("ERROR: diagonal '%s' not recognised.\n", diagonalStr.c_str());
		return 3;
	}

	if (diagonal == dCombined || diagonal == ad45b_56b || diagonal == ad45t_56t)
		return rcIncompatibleDiagonal;

	// set scenario
	env_sim = env;
	env_rec = env;
	anal_sim = anal;
	anal_rec = anal;

	if (SetScenario(scenario, biases, env_sim, anal_sim, env_rec, anal_rec) != 0)
	{
		printf("ERROR: unknown scenario '%s'.\n", scenario.c_str());
		return 2;
	}

	// print configuration
	printf(">> biases:\n");
	biases.Print();

	printf("\n\n>> env_sim:\n");
	env_sim.Print();

	printf("\n\n>> anal_sim:\n");
	anal_sim.Print();

	printf("\n\n>> anal_rec:\n");
	anal_rec.Print();

	// load non-gaussian distributions
	LoadNonGaussianDistributions(anal_sim.si_th_x_LRdiff, anal_sim.si_th_y_LRdiff);

	// load input dsigma/dt distribution
	if (LoadTDistributions() != 0)
		return 1;

	// initialise acceptance calculation
	accCalc.Init(th_y_sign, anal_rec);

	dist_true_t_spline = NULL;
	TGraph *g_dsdt = NULL;
   	for (const auto &itd : inputTDistributions)
	{
		if (itd.label == model)
		{
			dist_true_t_spline = itd.s_dsdt;
			g_dsdt = itd.g_dsdt;
		}
	}

	if (dist_true_t_spline == NULL)
	{
		printf("ERROR: dsigma/dt model '%s' not available.\n", model.c_str());
		return 5;
	}

	// prepare output
	TFile *f_out = new TFile(outFileName.c_str(), "recreate");

	g_dsdt->Write("g_dsdt");

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

	// sample distributions
	TGraph *g_dsdt_true = new TGraph(); g_dsdt_true->SetName("g_dsdt_true"); g_dsdt_true->SetLineColor(1);
	TGraph *g_dsdt_reco = new TGraph(); g_dsdt_reco->SetName("g_dsdt_reco"); g_dsdt_reco->SetLineColor(4);

	for (double t = 8E-4; t <= 1.0;)
	{
		//printf("            t = %.2E\n", t);

		const double v_true = dist_t_true(t);
		const double v_reco = dist_t_reco(t);

		int idx = g_dsdt_true->GetN();
		g_dsdt_true->SetPoint(idx, t, v_true);
		g_dsdt_reco->SetPoint(idx, t, v_reco);

		// advance t
		double dt = 0.005;
		if (t < 1.0) dt = 0.01;
		if (t < 0.6) dt = 0.005;
		if (t < 0.3) dt = 0.01;
		if (t < 0.2) dt = 0.005;
		if (t < 0.01) dt = 0.001;
		if (t < 4E-3) dt = 0.0001;
		if (t < 1.5E-3) dt = 0.00005;
		t += dt;
	}

	g_dsdt_true->Write();
	g_dsdt_reco->Write();

	delete f_out;

	return 0;
}
