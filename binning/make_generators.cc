#include "TFile.h"
#include "TProfile.h"
#include "TGraph.h"
#include "TF1.h"

#include <cmath>

//----------------------------------------------------------------------------------------------------

TGraph* MakeFixStatUncBinSizeGraph(TProfile *p_acc_corr, TF1 *ff_dNdt, double u, const char *name)
{
	TGraph *g_bs = new TGraph();
	g_bs->SetName(name);
	g_bs->SetTitle(";t;bin size");

	for (int i = 0; i < p_acc_corr->GetNbinsX(); i++)
	{
		double t = p_acc_corr->GetBinCenter(i);
		double acc_corr = p_acc_corr->GetBinContent(i);

		if (acc_corr == 0)
			continue;

		// there are 2 compensating "mistakes" originating from the same source:
		// 1) acc_corr is determined from a single diagonal, thus acc_corr >= 2
		//    and consquently A twice smaller than what it should be
		// 2) dNdt comes from merge which sums both diagonals instead of making
		//    average, thus making dNdt twice larger than what it should be
		// Nevertheless, the product A * dNdt is correct

		double A = 1./acc_corr;

		double dNdt = ff_dNdt->Eval(t);
		double bin_size = 1. / (u*u * dNdt * A);

		g_bs->SetPoint(g_bs->GetN(), t, bin_size);

		//printf("%.3f, dNdt=%E, A=%E, bin_size=%E\n", t, dNdt, A, bin_size);
	}

	return g_bs;
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// get input
	TFile *f_acc_corr = new TFile("acceptance_correction.root");
	TProfile *p_acc_corr = (TProfile *) f_acc_corr->Get("p_t_full_corr");

	TFile *f_dNdt = new TFile("dN_dt_fit.root");
	TF1 *ff_dNdt = (TF1 *) f_dNdt->Get("ff");

	/*
	{
		TH1D *h_dNdt = (TH1D *) f_dNdt->Get("h_dsdt");

		for (int i = 1; i <= h_dNdt->GetNbinsX(); ++i)
		{
			double c = h_dNdt->GetBinCenter(i);
			
			double v = h_dNdt->GetBinContent(i);
			double unc = h_dNdt->GetBinError(i);

			printf("i = %i, t = %.4f, rel unc = %.3f\n", i, c, unc/v);
		}
	}
	*/
	
	TFile *f_simu = new TFile("simu.root");
	TGraph *g_rms_t = (TGraph *) f_simu->Get("g_RMS_de_t_vs_t");

	// prepare output
	TFile *f_out = new TFile("generators.root", "recreate");

	g_rms_t->SetName("g_rms_t");
	g_rms_t->Write();

	MakeFixStatUncBinSizeGraph(p_acc_corr, ff_dNdt, 0.05, "g_bs_stat_unc_5")->Write();
	MakeFixStatUncBinSizeGraph(p_acc_corr, ff_dNdt, 0.10, "g_bs_stat_unc_10")->Write();
	MakeFixStatUncBinSizeGraph(p_acc_corr, ff_dNdt, 0.20, "g_bs_stat_unc_20")->Write();

	delete f_out;

	return 0;
}
