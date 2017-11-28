#include "TGraph.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TSpline.h"

#include <string>
#include <map>
#include <cstdio>

#include "common_definitions.h"
#include "common_algorithms.h"
#include "parameters.h"
#include "common.h"

#include "NumericalIntegration.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

unsigned long int_ws_phi_size, int_ws_MX_size, int_ws_MY_size;
gsl_integration_workspace *int_ws_phi, *int_ws_MX, *int_ws_MY;

//----------------------------------------------------------------------------------------------------

double n_sm_si;
double si_m_x, si_m_y;

//----------------------------------------------------------------------------------------------------

TSpline* BuildSpline(TGraph *g)
{
	TSpline3 *s = new TSpline3("", g->GetX(), g->GetY(), g->GetN());
	s->SetName(g->GetName());
	return s;
}

//----------------------------------------------------------------------------------------------------

double dist_m_x(double m)
{
	const double &si = si_m_x;
	return exp( - (m * m) / 2. / si / si ) / sqrt(2. * M_PI ) / si;
}

//----------------------------------------------------------------------------------------------------

double dist_m_y(double m_y)
{
	const double &si = si_m_y;
	return exp( - (m_y * m_y) / 2. / si / si ) / sqrt(2. * M_PI ) / si;
}

//----------------------------------------------------------------------------------------------------

TSpline *dist_t_true_spline = NULL;

// true distribution of t
double dist_t_true(double t)
{
	return dist_t_true_spline->Eval(t);
}

//----------------------------------------------------------------------------------------------------

// true distribution of th_x, th_y
double dist_th_x_th_y_true(double th_x, double th_y)
{
	double t = (th_x*th_x + th_y*th_y) * env.p*env.p;
	return dist_t_true(t);
}

//----------------------------------------------------------------------------------------------------

double IntegOverMX(double x, double *p, const void *)
{

	double th_x = p[0];
	double th_y = p[1];
	double m_x = x;
	double m_y = p[2];

	return dist_th_x_th_y_true(th_x - m_x, th_y - m_y) * dist_m_x(m_x);
}

//----------------------------------------------------------------------------------------------------

double IntegOverMY(double x, double *p, const void *)
{
	double m_y = x;
	double param[] = { p[0], p[1], m_y };

	double range_x = n_sm_si * si_m_x;
	double precision = 1E-2;
	double imx = RealIntegrate(IntegOverMX, param, NULL, -range_x, +range_x, 0., precision,
		int_ws_MX_size, int_ws_MX, "IntegOverMY");

	return imx * dist_m_y(m_y);
}

//----------------------------------------------------------------------------------------------------

double dist_th_x_th_y_smea(double th_x, double th_y)
{
	double param[] = { th_x, th_y };
	double range = n_sm_si * si_m_y;
	double precision = 1E-2;
	return RealIntegrate(IntegOverMY, param, NULL, -range, +range, 0., precision,
			int_ws_MY_size, int_ws_MY, "dist_th_x_th_y_sm");
}

//----------------------------------------------------------------------------------------------------

double IntegOverPhi(double x, double *p, const void *)
{
	double th = p[0];
	double phi = x;

	double th_x = th * cos(phi);
	double th_y = th * sin(phi);

	return dist_th_x_th_y_smea(th_x, th_y);
}

//----------------------------------------------------------------------------------------------------

double dist_t_sm(double t)
{
	double th = sqrt(t) / env.p;

	// get all intersections of const-th circle with acceptance boundaries
	set<double> phis;

	for (const auto &phi : anal.fc_G_l.GetIntersectionPhis(th))
		phis.insert(phi);

	for (const auto &phi : anal.fc_G_h.GetIntersectionPhis(th))
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
	double param[] = { th };
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

void MakeGraphs()
{
	TGraph *g_t_tr = new TGraph(); g_t_tr->SetName("g_t_tr"); g_t_tr->SetLineColor(1);
	TGraph *g_t_sm = new TGraph(); g_t_sm->SetName("g_t_sm"); g_t_sm->SetLineColor(2);
	TGraph *g_t_corr = new TGraph(); g_t_corr->SetName("g_t_corr"); g_t_sm->SetLineColor(4);

	for (double t = 5E-4; t <= 1.; )
	{
		// evaluate distributions
		const double d_t_true = dist_t_true(t);
		const double d_t_sm = dist_t_sm(t);
		const double corr = (d_t_sm > 0) ? d_t_true / d_t_sm : 0.;

		// fill plots
		int idx = g_t_tr->GetN();
		g_t_tr->SetPoint(idx, t, d_t_true);
		g_t_sm->SetPoint(idx, t, d_t_sm);

		g_t_corr->SetPoint(idx, t, corr);

		// advance t
		double dt = 0.01;
		if (t < 0.2)
			dt = 0.001;
		if (t < 0.004)
			dt = 0.0001;
		if (t < 0.001)
			dt = 0.00005;

		t += dt;
	}

	g_t_tr->Write();
	g_t_sm->Write();
	g_t_corr->Write();
}

//----------------------------------------------------------------------------------------------------

void MakeCorrectionHistograms(const vector<TH1D *> binning_hists)
{
	for (const TH1D *binning_hist : binning_hists)
	{
		TH1D *h_corr = new TH1D(*binning_hist);

		for (int bin = 1; bin <= h_corr->GetNbinsX(); bin++)
		{
			double l = h_corr->GetBinLowEdge(bin);
			double w = h_corr->GetBinWidth(bin);
			//double r = l + w;
	
			//printf("bin = %3u, l = %.5f, r = %.5f\n", bin, l, r);

			double v_corr;

			if (l+w < 3E-4 || l > 1.)
			{
				v_corr = 0.;
			} else {
				// integrate distributions over the bin
				double i_tr = 0., i_sm = 0.;
				unsigned int N_div = 10;
				for (unsigned int di = 0; di < N_div; di++)
				{
					double t = l + w/N_div * (di + 0.5);
					i_tr += dist_t_true(t);
					i_sm += dist_t_sm(t);
				}
			
				v_corr = (i_sm > 0.) ? i_tr / i_sm : 0.;
			}
	
			//printf("\t1-corr = %.3E | %.3E\n", 1.-corr, 1.-g_unsm_corr->Eval((l+r)/2));
	
			h_corr->SetBinContent(bin, v_corr);
		}

		h_corr->Write();
	}
}

//----------------------------------------------------------------------------------------------------

void ProcessOne(const vector<TH1D *> binning_hists)
{
	MakeGraphs();

	MakeCorrectionHistograms(binning_hists);
}

//----------------------------------------------------------------------------------------------------

struct Model
{
	string label;
	string file;
	string object;
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	if (argc < 2)
	{
		printf("ERROR: Wrong number of parameters.\n");
		return 1;
	}

	// init diagonal settings
	Init(argv[1]);
	if (diagonal == dCombined)
		return rcIncompatibleDiagonal;
	
	// binnings
	vector<string> binnings;
	//binnings.push_back("ub");
	binnings.push_back("ob-1-20-0.05");
	binnings.push_back("ob-2-10-0.05");
	binnings.push_back("ob-3-5-0.05");

	// models
	vector<Model> models = {
		{ "fitN-1", "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/coulomb_analysis_1/fits_for_corr/2500-2rp-ob-2-10-0.05/exp3,t_max=0.15,htv=1/fit.root", "g_fit_CH" },
		{ "fitN-2", "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/coulomb_analysis_1/fits_for_corr/2500-2rp-ob-2-10-0.05/exp3,t_max=0.15,htv=1/fit.root", "g_fit_CH" },
		{ "fitN-4", "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/coulomb_analysis_1/fits_for_corr/2500-2rp-ob-2-10-0.05/exp3,t_max=0.15,htv=1/fit.root", "g_fit_CH" },
	};

	// print info
	printf("\n");
	printf("------------------------------ environment ------------------------------\n");
	env.Print();
	printf("\n");
	printf("------------------------------- analysis --------------------------------\n");
	anal.Print();
	printf("\n");

	// smearing sigmas
	n_sm_si = 5;
	si_m_x = anal.si_th_x_2arm;
	si_m_y = anal.si_th_y_2arm;

	// build binnings
	vector<TH1D *> binning_hists;
	for (const auto &binning : binnings)
	{
		unsigned int N_bins;
		double *bin_edges;
		BuildBinning(anal, binning, bin_edges, N_bins, false);

		TH1D *h = new TH1D("", "", N_bins, bin_edges);
		h->SetName(binning.c_str());
		binning_hists.push_back(h);
	}

	// prepare output
	string fn_out = string("unfolding_cf_ni_") + argv[1] + ".root";
	TFile *f_out = new TFile(fn_out.c_str(), "recreate");
	if (f_out->IsZombie())
	{
		printf("ERROR: can't open file `%s' for writing.\n", fn_out.c_str());
		return 3;
	}
	
	// initialise integration workspaces
	int_ws_phi_size = 100;
	int_ws_phi = gsl_integration_workspace_alloc(int_ws_phi_size);

	int_ws_MX_size = 100;
	int_ws_MX = gsl_integration_workspace_alloc(int_ws_MX_size);

	int_ws_MY_size = 100;
	int_ws_MY = gsl_integration_workspace_alloc(int_ws_MY_size);

	// loop over data fits
	for (const auto model : models)
	{
		TDirectory *d_model = f_out->mkdir(model.label.c_str());

		// get input graph
		TFile *f_in = TFile::Open(model.file.c_str());
		TGraph *g_dsdt = (TGraph *) f_in->Get(model.object.c_str());

		// build spline
		dist_t_true_spline = BuildSpline(g_dsdt);

		// calculate correction
		gDirectory = d_model;

		ProcessOne(binning_hists);

		// clean up
		delete dist_t_true_spline;
		dist_t_true_spline = NULL;

		delete f_in;
	}
		
	delete f_out;

	return 0;
}
