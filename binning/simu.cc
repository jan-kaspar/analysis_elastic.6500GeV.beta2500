#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TProfile.h"
#include "TRandom2.h"
#include "TGraphErrors.h"

#include <cmath>

//----------------------------------------------------------------------------------------------------

void ProfileToRMSGraph(TProfile *p, TGraphErrors *g)
{
	for (int bi = 1; bi <= p->GetNbinsX(); ++bi)
	{
		double c = p->GetBinCenter(bi);

		double N = p->GetBinEntries(bi);
		double Sy = p->GetBinContent(bi) * N;
		double Syy = p->GetSumw2()->At(bi);
		
		double si_sq = Syy/N - Sy*Sy/N/N;
		double si = (si_sq >= 0.) ? sqrt(si_sq) : 0.;
		double si_unc_sq = si_sq / 2. / N;	// Gaussian approximation
		double si_unc = (si_unc_sq >= 0.) ? sqrt(si_unc_sq) : 0.;

		int idx = g->GetN();
		g->SetPoint(idx, c, si);
		g->SetPointError(idx, 0., si_unc);
	}
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// settings
	double p = 6.5E3;	// GeV

	double si_de_th_x_2arm = 0.29E-6;
	double si_de_th_y_1arm = 0.26E-6;

	double lcut_th_y_L = 4.3E-6;
	double lcut_th_y_R = 4.3E-6;
	double hcut_th_y_L = 100E-6;
	double hcut_th_y_R = 100E-6;

	// get input
	TFile *f_in = TFile::Open("dN_dt_fit.root");
	TF1 *f_dN_dt = (TF1 *) f_in->Get("ff");

	// prepare output
	TFile *f_out = TFile::Open("simu.root", "recreate");

	TH1D *h_th_x_tr = new TH1D("h_th_x_tr", ";#theta_{x}^{*}  (#murad)", 200, -200E-6, -200E-6);
	TH1D *h_th_y_tr = new TH1D("h_th_y_tr", ";#theta_{y}^{*}  (#murad)", 200, -200E-6, -200E-6);
	TH1D *h_t_tr = new TH1D("h_t_tr", ";t  (GeV^{2})", 100, 0., 3.0);
	
	TH1D *h_th_x_sm = new TH1D("h_th_x_sm", ";#theta_{x}^{*}  (#murad)", 200, -200E-6, -200E-6);
	TH1D *h_th_y_sm = new TH1D("h_th_y_sm", ";#theta_{y}^{*}  (#murad)", 200, -200E-6, -200E-6);
	TH1D *h_t_sm = new TH1D("h_t_sm", ";t  (GeV^{2})", 100, 0., 3.0);

	TH2D *h_de_t_vs_t = new TH2D("h_de_t_vs_t", ";t  (GeV^{2});#Delta t  (GeV^{2})", 100, 0., 1.0, 100, 0., 0.);
	TProfile *p_de_t_vs_t = new TProfile("p_de_t_vs_t", ";t  (GeV^{2})", 1000, 0., 1.0);

	// simulation loop
	for (unsigned int i = 0; i < 100000000; i++)
	{
		double t = gRandom->Rndm() * 1.2;
		double phi = gRandom->Rndm() * M_PI;

		double w = f_dN_dt->Eval(t);

		double th = sqrt(t) / p;
		double th_x = th * cos(phi);
		double th_y = th * sin(phi);

		h_th_x_tr->Fill(th_x, w);
		h_th_y_tr->Fill(th_y, w);
		h_t_tr->Fill(t, w);

		double th_y_sm_L = th_y + gRandom->Gaus() * si_de_th_y_1arm;
		double th_y_sm_R = th_y + gRandom->Gaus() * si_de_th_y_1arm;
		double th_y_sm = (th_y_sm_L + th_y_sm_R) / 2.;

		double th_x_sm = th_x + gRandom->Gaus() * si_de_th_x_2arm;

		double t_sm = p*p * (th_x_sm * th_x_sm + th_y_sm * th_y_sm);

		double de_t = t_sm - t;

		if (th_y_sm_L < lcut_th_y_L || th_y_sm_L > hcut_th_y_L)
			continue;

		if (th_y_sm_R < lcut_th_y_R || th_y_sm_R > hcut_th_y_R)
			continue;

		h_th_x_sm->Fill(th_x_sm, w);
		h_th_y_sm->Fill(th_y_sm, w);
		h_t_sm->Fill(t_sm, w);

		h_de_t_vs_t->Fill(t, de_t);
		p_de_t_vs_t->Fill(t, de_t);
	}
	
	h_th_x_tr->Write();
	h_th_y_tr->Write();
	h_t_tr->Write();
	
	h_th_x_sm->Write();
	h_th_y_sm->Write();
	h_t_sm->Write();
	
	h_de_t_vs_t->Write();
	p_de_t_vs_t->Write();

	TGraphErrors *g_RMS_de_t_vs_t = new TGraphErrors();
	g_RMS_de_t_vs_t->SetName("g_RMS_de_t_vs_t");
	ProfileToRMSGraph(p_de_t_vs_t, g_RMS_de_t_vs_t);
	g_RMS_de_t_vs_t->Write();

	delete f_out;
}
