#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TF1.h"
#include "TMath.h"

#include <algorithm>
#include <set>
#include <map>

#include "numerical_integration_common.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

double p = 4E3;	// GeV

double th_y_sign;

double th_y_lcut_L, th_y_lcut_R;
double th_y_hcut_L, th_y_hcut_R;
double th_y_lcut;
double th_y_hcut;

double th_x_lcut;
double th_x_hcut;

double si_de_th_x_2arm_cen;	// central value
double si_de_th_y_1arm_cen;

double si_de_th_x_2arm;		// value actually used in calculations
double si_de_th_y_1arm;

//----------------------------------------------------------------------------------------------------

double si_de_th_y_1arm_unc;	// offset to 1-arm si_de_th_y, rad
double RL_asymmetry;

double si_de_th_x_2arm_time_indep_unc;
double si_de_th_y_1arm_time_indep_unc;

//----------------------------------------------------------------------------------------------------

unsigned int de_th_y_dist_type;

string DeThYDistTypeName(unsigned int t)
{
	if (t == 0)
		return "standard";

	if (t == 1)
		return "sigma offset";

	if (t == 2)
		return "sigma asymmetry";

	if (t == 3)
		return "non-gauss";

	return "unknown";
}

//----------------------------------------------------------------------------------------------------

TF1 *f_dist_de_th_y = NULL;

double dist_de_th_y(double de_th_y, bool left)
{
	if (de_th_y_dist_type == 0)
	{
		double si = si_de_th_y_1arm;
		return exp( - (de_th_y * de_th_y) / 2. / si / si ) / sqrt(2. * M_PI ) / si;
	}

	if (de_th_y_dist_type == 1)
	{
		double si = si_de_th_y_1arm + si_de_th_y_1arm_unc;
		return exp( - (de_th_y * de_th_y) / 2. / si / si ) / sqrt(2. * M_PI ) / si;
	}

	if (de_th_y_dist_type == 2)
	{
		double si = (left) ? si_de_th_y_1arm * sqrt(1. + RL_asymmetry) : si_de_th_y_1arm / sqrt(1. + RL_asymmetry);
		return exp( - (de_th_y * de_th_y) / 2. / si / si ) / sqrt(2. * M_PI ) / si;
	}

	if (de_th_y_dist_type == 3)
	{
		return f_dist_de_th_y->Eval(de_th_y);
	}

	return 0.;
}

//----------------------------------------------------------------------------------------------------

double dist_de_th_y_L(double de_th_y)
{

	return dist_de_th_y(de_th_y, true);
}

//----------------------------------------------------------------------------------------------------

double dist_de_th_y_R(double de_th_y)
{
	return dist_de_th_y(de_th_y, false);
}

//----------------------------------------------------------------------------------------------------

double dist_th_x_th_y_true(double th_x, double th_y)
{
	double t = (th_x*th_x + th_y*th_y) * p*p;
	return dist_true_t(t);
}

//----------------------------------------------------------------------------------------------------

double IntegOverDeThX(double x[], double p[])
{
	double de_th_x = x[0];

	double th_x_p = p[0];
	double th_y_p = p[1];

	double si = si_de_th_x_2arm;
	double g_de_th_x = exp( - (de_th_x * de_th_x) / 2. / si / si ) / sqrt(2. * M_PI ) / si;

	return dist_th_x_th_y_true(th_x_p - de_th_x, th_y_p) * g_de_th_x;
}

//----------------------------------------------------------------------------------------------------

double dist_th_x_th_y_x_sm_acc(double th_x, double th_y)
{
	if (th_x < th_x_lcut && th_x > th_x_hcut)
		return 0;

	//no th_x smearing
	//return dist_th_x_th_y_true(th_x, th_y);

	//double precision = 1E-5;
	double precision = 1E-3;
	double range = 5. * si_de_th_x_2arm;
	double param[] = { th_x, th_y };

	return DoubleInt(IntegOverDeThX, -range, +range, param, precision);
}

//----------------------------------------------------------------------------------------------------

double IntegOverD(double x[], double p[])
{
	double d = x[0];

	double m = p[0];
	//double th_x_p = p[1];
	//double th_y_p = p[2];

	return dist_de_th_y_L(m - d/2.) * dist_de_th_y_R(m + d/2.);
}

//----------------------------------------------------------------------------------------------------

bool applyAcceptanceCorrection = false;

double IntegOverM(double x[], double p[])
{
	double m = x[0];
	double th_x_p = p[0];
	double th_y_p = p[1];

	double param[3] = { m, th_x_p, th_y_p };

	double s_th_y_hcut_L, s_th_y_lcut_R, s_th_y_lcut_L, s_th_y_hcut_R;
	if (th_y_sign > 0)
	{
		s_th_y_lcut_L = th_y_lcut_L;
		s_th_y_hcut_L = th_y_hcut_L;
		s_th_y_lcut_R = th_y_lcut_R;
		s_th_y_hcut_R = th_y_hcut_R;
	} else {
		s_th_y_lcut_L = -th_y_hcut_L;
		s_th_y_hcut_L = -th_y_lcut_L;
		s_th_y_lcut_R = -th_y_hcut_R;
		s_th_y_hcut_R = -th_y_lcut_R;
	}

	double d_min = 2. * max(th_y_p - s_th_y_hcut_L, s_th_y_lcut_R - th_y_p);
	double d_max = 2. * min(th_y_p - s_th_y_lcut_L, s_th_y_hcut_R - th_y_p);

	double d_integral;
	if (d_max <= d_min)
		d_integral = 0.;
	else
	{
		//double typical_sigma = 20E-6;
		double typical_sigma = 10E-6;
		//double precision = 1E-4;
		double precision = 1E-3;
		
		d_integral = 0.;

		if (d_max > typical_sigma)
		{
			d_integral += DoubleInt(IntegOverD, 0., typical_sigma, param, precision);
			d_integral += DoubleInt(IntegOverD, typical_sigma, d_max, param, precision);
		} else
			d_integral += DoubleInt(IntegOverD, 0., d_max, param, precision);

		if (d_min < -typical_sigma)
		{
			d_integral += DoubleInt(IntegOverD, -typical_sigma, 0., param, precision);
			d_integral += DoubleInt(IntegOverD, d_min, -typical_sigma, param, precision);
		} else
			d_integral += DoubleInt(IntegOverD, d_min, 0., param, precision);
	}

	//printf("\t\tm = %+.2f urad | d_min = %.4f, d_max = %.4f urad | d_integral = %.1E\n",
	//.	m*1E6, d_min*1E6, d_max*1E6, d_integral);
	
	double div_corr = 1.;

	if (applyAcceptanceCorrection)
	{
		double LB_y = d_min / 2.;
		double UB_y = d_max / 2.;
		double F_y = (UB_y > LB_y) ? ( TMath::Erf(UB_y / si_de_th_y_1arm) - TMath::Erf(LB_y / si_de_th_y_1arm) ) / 2. : 0.;
		div_corr = 1. / F_y;
		//printf("F_y = %E, div_corr = %E\n", F_y, div_corr);
	}

	return dist_th_x_th_y_x_sm_acc(th_x_p, th_y_p - m) * d_integral * div_corr;
}

//----------------------------------------------------------------------------------------------------

double dist_th_x_th_y_obs(double th_x_p, double th_y_p)
{
	double range_m = 4E-6;
	double param[2] = { th_x_p, th_y_p };

	//double precision = 1E-4;
	double precision = 1E-3;

	double result;
	if (th_y_sign * th_y_p < th_y_lcut || th_y_sign * th_y_p > th_y_hcut)
		result = 0.;
	else
		result = DoubleInt(IntegOverM, -range_m, +range_m, param, precision);

	//printf("\tresult = %E\n", result);

	return result;
}

//----------------------------------------------------------------------------------------------------

double IntegOverPhi(double x[], double par[])
{
	double phi = x[0];
	double th_p = par[0];

	double th_x_p = th_p * cos(phi);
	double th_y_p = th_p * sin(phi);

	//double result = (use_reco_dist_th_x_th_y) ? dist_reco_th_x_th_y(th_x_p, th_y_p) : dist_true_th_x_th_y(th_x_p, th_y_p);
	double result = dist_th_x_th_y_obs(th_x_p, th_y_p);
	//printf("phi = %.3f => %.5E\n", phi, result);

	return result;
}

//----------------------------------------------------------------------------------------------------

double dist_t_obs(double t_p)
{
	double th_p = sqrt(t_p) / p;
	double param[1] = { th_p };

	// get all intersections of const-th circle with acceptance boundaries
	set<double> phis;

	if (th_p > th_y_lcut)
	{
		double phi = asin(th_y_lcut / th_p);
		double ta_x = th_p * cos(phi);
		if (th_x_lcut < ta_x && ta_x < th_x_hcut)
			phis.insert(phi);
		if (th_x_lcut < -ta_x && -ta_x < th_x_hcut)
			phis.insert(M_PI - phi);
	}
	
	if (th_p > th_y_hcut)
	{
		double phi = asin(th_y_hcut / th_p);
		double ta_x = th_p * cos(phi);
		if (th_x_lcut < ta_x && ta_x < th_x_hcut)
			phis.insert(phi);
		if (th_x_lcut < -ta_x && -ta_x < th_x_hcut)
			phis.insert(M_PI - phi);
	}

	if (th_p > fabs(th_x_hcut))
	{
		double phi = acos(fabs(th_x_hcut) / th_p);
		double ta_y = th_p * sin(phi);
		if (th_y_lcut < ta_y && ta_y < th_y_hcut)
			phis.insert(phi);
	}

	if (th_p > fabs(th_x_lcut))
	{
		double phi = acos(fabs(th_x_lcut) / th_p);
		double ta_y = th_p * sin(phi);
		if (th_y_lcut < ta_y && ta_y < th_y_hcut)
			phis.insert(M_PI - phi);
	}

	// the number of intersections must be even
	if ((phis.size() % 2) == 1)
	{
		printf("ERROR: odd number of intersections in acceptance calculation\n");
	}

	// no intersection => no acceptances
	if (phis.size() == 0)
		return 0.;

	// calculate integrals over phi sections
	//double precision = 1E-3;
	double precision = 1E-4;
	double phiSum = 0.;
	double integralSum = 0.;
	for (set<double>::iterator it = phis.begin(); it != phis.end(); ++it)
	{
		double phi_start = *it;
		++it;
		double phi_end = *it;

		phiSum += phi_end - phi_start;

		if (th_y_sign == +1)
			integralSum += DoubleInt(IntegOverPhi, phi_start, phi_end, param, precision);
		else
			integralSum += DoubleInt(IntegOverPhi, -phi_end, -phi_start, param, precision);
	}

	if (applyAcceptanceCorrection)
		return integralSum / phiSum;
	else
		return integralSum;
}

//----------------------------------------------------------------------------------------------------

void InitNonGaussianDeThYDist()
{
	printf(">> InitNonGaussianDeThYDist\n");
	printf("\tsi_de_th_y_1arm = %E\n", si_de_th_y_1arm);

	// a double-gaussian fit of experimental th_y_R - th_y_L distribution
	string formula = "[0] * exp( - (x-[1])^2 / 2. / [2]^2) + [3] * exp( - (x-[4])^2 / 2. / [5]^2)";
	TF1 *f_test = new TF1("f_test", formula.c_str());
	f_test->SetParameters(
			8.30073e+11, 0., 7.93807e-07,
			-8.09176e+11, 0., 7.88737e-07
		);

	// normalise it to integral of one
	double In = f_test->Integral(-5E-6, +5E-6);
	f_test->SetParameter(0, f_test->GetParameter(0) / In);
	f_test->SetParameter(3, f_test->GetParameter(3) / In);
	double Iv = f_test->Integral(-30E-6, +30E-6);
	//printf("%E\n", Iv);

	// calculate RMS
	TF1 *f_test_rms = new TF1("f_test_rms", ("(" + formula + ")*x*x").c_str());
	for (int i = 0; i < f_test->GetNpar(); i++)
		f_test_rms->SetParameter(i, f_test->GetParameter(i));

	// normalise width to the standard RMS - si_de_th_y_1arm
	double I = f_test_rms->Integral(-5E-6, +5E-6);
	printf("\tRMS before = %E\n", sqrt(I));

	double r = si_de_th_y_1arm / sqrt(I);

	f_test->SetParameter(2, f_test->GetParameter(2) * r);
	f_test->SetParameter(5, f_test->GetParameter(5) * r);
	
	// (vertical) normalisation needs to be renormalised too
	In = f_test->Integral(-5E-6, +5E-6);
	f_test->SetParameter(0, f_test->GetParameter(0) / In);
	f_test->SetParameter(3, f_test->GetParameter(3) / In);
	for (int i = 0; i < f_test->GetNpar(); i++)
		f_test_rms->SetParameter(i, f_test->GetParameter(i));

	Iv = f_test_rms->Integral(-5E-6, +5E-6);
	printf("\tRMS after = %E\n", sqrt(Iv));

	f_dist_de_th_y = f_test;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

TGraph* MakeRatio(TGraph *num, TGraph *den, const char *name, int color)
{
	TGraph *r = new TGraph(*num);
	r->SetName(name);
	r->SetLineColor(color);

	for (int i = 0; i < r->GetN(); i++)
	{
		double x, y_n, y_d;
		num->GetPoint(i, x, y_n);
		den->GetPoint(i, x, y_d);

		double y_r = (y_d != 0.) ? y_n / y_d : 0.;

		r->SetPoint(i, x, y_r);
	}

	return r;
}

//----------------------------------------------------------------------------------------------------

enum DSType { dsUnknown, ds2a, ds2b } ds = dsUnknown; 
enum DGNType { dgnUnknown, dgn_45b, dgn_45t } dgn = dgnUnknown;

int Setup(const string &s_dataset, const string &s_diagonal)
{
	// parse dataset and diagonal
	if (s_dataset.compare("DS2a") == 0) ds = ds2a;
	if (s_dataset.compare("DS2b") == 0) ds = ds2b;

	if (ds == dsUnknown)
	{
		printf("ERROR: dataset `%s' not recognized.\n", s_dataset.c_str());
		return 1;
	}

	if (s_diagonal.compare("45b_56t") == 0) dgn = dgn_45b;
	if (s_diagonal.compare("45t_56b") == 0) dgn = dgn_45t;

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
	
	// acceptance cuts
	th_x_lcut = -50E-6;
	th_x_hcut = +80E-6;

	if (dgn == dgn_45b)
	{
		th_y_lcut_L = 13.8E-6; th_y_lcut_R = 13.8E-6;
		th_y_hcut_L = 100E-6; th_y_hcut_R = 100E-6;
		th_y_lcut = max(th_y_lcut_L, th_y_lcut_R) + 0.2E-6;
		th_y_hcut = min(th_y_hcut_L, th_y_hcut_R) - 1.0E-6;
	}

	if (dgn == dgn_45t)
	{
		th_y_lcut_L = 5.8E-6; th_y_lcut_R = 5.8E-6;
		th_y_hcut_L = 100E-6; th_y_hcut_R = 100E-6;
		th_y_lcut = max(th_y_lcut_L, th_y_lcut_R) + 0.2E-6;
		th_y_hcut = min(th_y_hcut_L, th_y_hcut_R) - 1.0E-6;
	}
	
	// resolution parameters
	if (ds == ds2a)
	{
		// middle of of run 8554
		si_de_th_y_1arm_cen = 0.88E-6 / sqrt(2.);

		double bd = 0.77E-6;	
		double pc = 0.11E-6;
		si_de_th_x_2arm_cen = sqrt(bd*bd/2. + pc*pc);
	}

	if (ds == ds2b)
	{
		// middle of of run 8560
		si_de_th_y_1arm_cen = 0.95E-6 / sqrt(2.);
		
		double bd = 0.88E-6;	
		double pc = 0.11E-6;
		si_de_th_x_2arm_cen = sqrt(bd*bd/2. + pc*pc);
	}

	// time-dependent determination uncertainty
	si_de_th_y_1arm_unc = 0.02E-6;	// urad
	
	RL_asymmetry = 0.25;			// 1

	// time-independent uncerainty (band half-width)
	si_de_th_x_2arm_time_indep_unc = 0.08E-6 / sqrt(2.);	// urad
	si_de_th_y_1arm_time_indep_unc = 0.05E-6 / sqrt(2.);	// urad

	return 0;
}

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: numerical_integration_double <dataset> <diagonal> <output file name>\n");
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

	// load input dsigma/dt distribution
	if (LoadTDistributions() != 0)
		return 1;
	
	// prepare output
	TFile *f_out = new TFile(outFileName.c_str(), "recreate");
	
	WriteTDistributions();

	/*
	TGraph *g = new TGraph();
	double th_x = 20E-6;
	double w_th_y = 100E-6;
	for (double th_y = th_y_lcut - 1E-6; th_y < th_y_hcut + 1E-6; th_y += w_th_y)
	//for (double th_y = 42E-6; th_y < 44E-6; th_y += w_th_y)
	{
		if (th_y > th_y_lcut - 2E-6)
			w_th_y = 0.01E-6;

		if (th_y > th_y_lcut + 1E-6)
			w_th_y = 0.1E-6;

		if (th_y > th_y_lcut + 2E-6)
			w_th_y = 2E-6;

		if (th_y > 95E-6)
			w_th_y = 0.1E-6;

		printf("th_y = %.2f urad\n", th_y * 1E6);

		double v = dist_th_x_th_y_obs(th_x, th_y);

		int idx = g->GetN();
		g->SetPoint(idx, th_y, v);
	}
	*/

	// buffer for unsmearing corrections (with no systematics)
	// t-dist. type index --> (sigma setting index --> unsmearing correction graph)
	map<unsigned int, map<unsigned int, TGraph *> > unsmearing_corrections;

	// t-distributions
	for (unsigned int ssi = 0; ssi < 3; ++ssi)
	{
		string ssi_label;
		if (ssi == 0)
		{
			ssi_label="x+0,y+0";
			si_de_th_x_2arm = si_de_th_x_2arm_cen + 0.*si_de_th_x_2arm_time_indep_unc;
			si_de_th_y_1arm = si_de_th_y_1arm_cen + 0.*si_de_th_y_1arm_time_indep_unc;
		}
		if (ssi == 1)
		{
			ssi_label="x+1,y+0";
			si_de_th_x_2arm = si_de_th_x_2arm_cen + 1.*si_de_th_x_2arm_time_indep_unc;
			si_de_th_y_1arm = si_de_th_y_1arm_cen + 0.*si_de_th_y_1arm_time_indep_unc;
		}
		if (ssi == 2)
		{
			ssi_label="x+0,y+1";
			si_de_th_x_2arm = si_de_th_x_2arm_cen + 0.*si_de_th_x_2arm_time_indep_unc;
			si_de_th_y_1arm = si_de_th_y_1arm_cen + 1.*si_de_th_y_1arm_time_indep_unc;
		}

		printf(">> sigma settings: %s\n", ssi_label.c_str());
		printf("\tsi_de_th_x_2arm = %E\n", si_de_th_x_2arm);
		printf("\tsi_de_th_y_1arm = %E\n", si_de_th_y_1arm);

		// prepare non-gaussian de_th_y distribution
		InitNonGaussianDeThYDist();

		TDirectory *ssiDir = f_out->mkdir(ssi_label.c_str());

		vector<unsigned int> t_dist_types;
		t_dist_types.push_back(0);
		//t_dist_types.push_back(2);
		t_dist_types.push_back(3);
		t_dist_types.push_back(4);
		t_dist_types.push_back(5);

		for (unsigned int tdi = 0; tdi < t_dist_types.size(); tdi++)
		{
			t_dist_type = t_dist_types[tdi];
			string t_dist_label = TDistTypeName(t_dist_type);
			printf(">> input t-distribution: %s\n", t_dist_label.c_str());
	
			TDirectory *tDistDir = ssiDir->mkdir(t_dist_label.c_str());
			gDirectory = tDistDir;
	
			// sample true t-distribution
			TGraph *g_t_dist_true = new TGraph();
			g_t_dist_true->SetName("g_t_dist_true");
		
			double w_t = 100.;
			double t_min = pow(th_y_lcut*p, 2.);
			for (double t = t_min; t < 0.25; t += w_t)
			{
				if (t > 0.)
					//w_t = 0.02*t;
					w_t = 0.05*t;
			
				if (t > 2.*t_min)
					//w_t = 0.05*t;
					w_t = 0.30*t;
				
				if (t > 0.01)
					//w_t = 0.01;
					w_t = 0.025;
			
				printf("\tt = %.4f GeV^2\n", t);
			
				double v = dist_true_t(t);
			
				int idx = g_t_dist_true->GetN();
				g_t_dist_true->SetPoint(idx, t, v);
			}
		
			g_t_dist_true->Write();
		
			// sample observable t-distributions
			map<unsigned int, TGraph *> t_dists_obs;
			map<unsigned int, TGraph *> t_dists_reco;
		
			unsigned int max_di = 4;
			if (ssi > 0)
				max_di = 1;
			for (unsigned int di = 0; di < max_di; di++)
			{
				de_th_y_dist_type = di;
				string label = DeThYDistTypeName(di);
				printf("\t%s\n", label.c_str());
		
				TGraph *g_obs = new TGraph(); g_obs->SetName("t_dist_obs");
				TGraph *g_reco = new TGraph(); g_reco->SetName("t_dist_reco");
		
				double *t_array = g_t_dist_true->GetX();
				for (int idx = 0; idx < g_t_dist_true->GetN(); idx++)
				{
					double t = t_array[idx];
	
					applyAcceptanceCorrection = false;
					g_obs->SetPoint(idx, t, dist_t_obs(t));
					
					// speed optimisation
					if (di == 0)
					{
						applyAcceptanceCorrection = true;
						g_reco->SetPoint(idx, t, dist_t_obs(t));
					}
				}
				
				t_dists_obs[di] = g_obs;
				t_dists_reco[di] = g_reco;
			}
		
			// save ratios (relative effects)
			TGraph *g_std_obs = t_dists_obs[0];
	
			for (map<unsigned int, TGraph *>::iterator it = t_dists_obs.begin(); it != t_dists_obs.end(); ++it)
			{
				unsigned int di = it->first;
				gDirectory = tDistDir->mkdir(DeThYDistTypeName(di).c_str());
	
				t_dists_obs[di]->Write();
				t_dists_reco[di]->Write();
	
				TGraph *g_unsm_corr = MakeRatio(g_t_dist_true, t_dists_reco[di], "unsmearing correction", 8);
				g_unsm_corr->Write();
	
				TGraph *g_rel = MakeRatio(it->second, g_std_obs, "systematic effect", 2);
				g_rel->Write();

				// standard = no systematics
				if (it->first == 0)
					unsmearing_corrections[t_dist_types[tdi]][ssi] = g_unsm_corr;
			}
		}
	}

	// calculate uncertainties of unsmearing correction
	TDirectory *unsmCorrDir = f_out->mkdir("unsmearing correction");

	int graph_points = 0;

	for (map<unsigned int, map<unsigned int, TGraph *> >::iterator tdit = unsmearing_corrections.begin();
			tdit != unsmearing_corrections.end(); tdit++)
	{
		TDirectory *tdDir = unsmCorrDir->mkdir(TDistTypeName(tdit->first).c_str());
		gDirectory = tdDir;

		TGraph *unsm_corr_base = tdit->second[0];
		TGraph *unsm_corr_th_x = tdit->second[1];
		TGraph *unsm_corr_th_y = tdit->second[2];

		TGraph *unsm_corr_unc_th_x = new TGraph(*unsm_corr_base); unsm_corr_unc_th_x->SetName("unsm_corr_unc_th_x");
		TGraph *unsm_corr_unc_th_y = new TGraph(*unsm_corr_base); unsm_corr_unc_th_y->SetName("unsm_corr_unc_th_y");

		graph_points = unsm_corr_base->GetN();
		for (int i = 0; i < graph_points; i++)
		{
			double t, c_b, c_x, c_y;
			unsm_corr_base->GetPoint(i, t, c_b);
			unsm_corr_th_x->GetPoint(i, t, c_x);
			unsm_corr_th_y->GetPoint(i, t, c_y);

			unsm_corr_unc_th_x->SetPoint(i, t, c_x - c_b);
			unsm_corr_unc_th_y->SetPoint(i, t, c_y - c_b);
		}

		unsm_corr_unc_th_x->Write();
		unsm_corr_unc_th_y->Write();
	}
	
	gDirectory = unsmCorrDir;

	TGraph *unsm_corr_unc_model = new TGraph(); unsm_corr_unc_model->SetName("unsm_corr_unc_model");
	for (int i = 0; i < graph_points; i++)
	{
		double t=0;
		double S1=0, Sc=0, Scc=0;
		for (map<unsigned int, map<unsigned int, TGraph *> >::iterator tdit = unsmearing_corrections.begin();
				tdit != unsmearing_corrections.end(); tdit++)
		{
			// selection of t-distribution models
			if (tdit->first < 2)
				continue;

			TGraph *corr = tdit->second[0];
			double c;
			corr->GetPoint(i, t, c);

			S1++;
			Sc += c;
			Scc += c*c;
		}

		double var = (S1 > 1.) ? (Scc - Sc*Sc/S1) / (S1 - 1.) : 0.;
		double stddev = (var >= 0.) ? sqrt(var) : 0.;

		unsm_corr_unc_model->SetPoint(i, t, stddev);
	}

	unsm_corr_unc_model->Write();

	delete f_out;

	return 0;
}
