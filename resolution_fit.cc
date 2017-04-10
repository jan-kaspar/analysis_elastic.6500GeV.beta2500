#include "common_definitions.h"
#include "common_algorithms.h"
#include "parameters.h"
#include "common.h"

#include "TFile.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TProfile.h"

#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

void RunOneFit(const TGraph *g_run_boundaries, const TProfile *p, const TGraphErrors *g_rms, double unc_th)
{
	// local fit function
	TF1 *ff = new TF1("ff", "1 ++ x");

	// graph of fit results
	TGraph *g_fits = new TGraph();
	g_fits->SetName("g_fits");
	g_fits->SetTitle(";timestamp;RMS");

	// loop over runs
	for (int rgi = 0; rgi < g_run_boundaries->GetN();)
	{
		// get run boundaries
		double run, run_beg, run_end;
		g_run_boundaries->GetPoint(rgi, run_beg, run);
		g_run_boundaries->GetPoint(rgi+1, run_end, run);
		rgi += 2;

		printf("run = %.0f\n", run);

		// adjust boundaries to source profile
		int bi_beg = p->GetXaxis()->FindBin(run_beg);
		int bi_end = p->GetXaxis()->FindBin(run_end);

		double t_min = p->GetXaxis()->GetBinLowEdge(bi_beg);
		double t_max = p->GetXaxis()->GetBinLowEdge(bi_end) + p->GetXaxis()->GetBinWidth(bi_end);

		printf("    t_min = %.0f, t_max = %.0f\n", t_min, t_max);

		// copy selected points
		TGraphErrors *g_rms_sel = new TGraphErrors();

		for (int pi = 0; pi < g_rms->GetN(); ++pi)
		{
			double t, rms, rms_u;
			g_rms->GetPoint(pi, t, rms);
			rms_u = g_rms->GetErrorY(pi);

			if (t < t_min || t > t_max)
				continue;

			if (rms_u > unc_th)
				continue;

			int idx = g_rms_sel->GetN();
			g_rms_sel->SetPoint(idx, t, rms);
			g_rms_sel->SetPointError(idx, 0., rms_u);
		}

		printf("    points selected: %i\n", g_rms_sel->GetN());

		// data sufficient?
		if (g_rms_sel->GetN() < 4)
			continue;

		// make fit
		g_rms_sel->Fit(ff, "Q");

		// add points to g_fit
		const double a = ff->GetParameter(1);
		const double b = ff->GetParameter(0);

		int idx = g_fits->GetN();
		g_fits->SetPoint(idx, run_beg, a*run_beg + b);
		g_fits->SetPoint(idx+1, run_end, a*run_end + b);

		// clean up
		delete g_rms_sel;
	}

	// save output
	g_fits->Write();

	// clean up
	delete ff;
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc != 2)
		return 1;

	// init diagonal
	Init(argv[1]);
	if (diagonal == dCombined || diagonal == ad45b_56b || diagonal == ad45t_56t)
		return rcIncompatibleDiagonal;

	// get input
	TFile *f_in = new TFile((string("distributions_") + argv[1] + ".root").c_str());

	TGraph *g_run_boundaries = (TGraph *) f_in->Get("time dependences/g_run_boundaries");
	
	TProfile *p_diffLR_th_x_vs_time = (TProfile *) f_in->Get("time dependences/p_diffLR_th_x_vs_time");
	TGraphErrors *gRMS_diffLR_th_x_vs_time = (TGraphErrors *) f_in->Get("time dependences/gRMS_diffLR_th_x_vs_time");
	
	TProfile *p_diffLR_th_y_vs_time = (TProfile *) f_in->Get("time dependences/p_diffLR_th_y_vs_time");
	TGraphErrors *gRMS_diffLR_th_y_vs_time = (TGraphErrors *) f_in->Get("time dependences/gRMS_diffLR_th_y_vs_time");
	
	// prepare output
	TFile *f_out = new TFile((string("resolution_fit_") + argv[1] + ".root").c_str(), "recreate");

	// do fits
	printf("\n\n---------- d_x ----------\n");
	gDirectory = f_out->mkdir("d_x");
	RunOneFit(g_run_boundaries, p_diffLR_th_x_vs_time, gRMS_diffLR_th_x_vs_time, 0.3E-6);

	printf("\n\n---------- d_y ----------\n");
	gDirectory = f_out->mkdir("d_y");
	RunOneFit(g_run_boundaries, p_diffLR_th_y_vs_time, gRMS_diffLR_th_y_vs_time, 0.01E-6);

	// clean up
	delete f_out;
	delete f_in;
	return 0;
}
