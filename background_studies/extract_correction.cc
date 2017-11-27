#include "TFile.h"
#include "TGraph.h"
#include "TF1.h"
#include "TH1D.h"
#include "TCanvas.h"

#include <string>
#include <vector>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

int main()
{
	// configuration
	string dataset = "DS-fill5317";

	// get input
	TFile *f_in = TFile::Open((dataset + "/ratios.root").c_str());

	TH1D *h_bb_bt = (TH1D *) f_in->Get("anti_45b_56b_over_45b_56t");
	TH1D *h_tt_bt = (TH1D *) f_in->Get("anti_45t_56t_over_45b_56t");
	TH1D *h_bb_tb = (TH1D *) f_in->Get("anti_45b_56b_over_45t_56b");
	TH1D *h_tt_tb = (TH1D *) f_in->Get("anti_45t_56t_over_45t_56b");

	h_bb_bt->SetName("h_bb_bt");
	h_tt_bt->SetName("h_tt_bt");
	h_bb_tb->SetName("h_bb_tb");
	h_tt_tb->SetName("h_tt_tb");

	// prepare output
	TFile *f_out = TFile::Open("correction.root", "recreate");

	TGraph *g_high_t_in = new TGraph();
	g_high_t_in->SetName("g_high_t_in");
	g_high_t_in->SetLineStyle(2);

	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.000, 0.0035);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.002, 0.002);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.005, 0.0013);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.010, 0.001);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.015, 0.0009);

	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.05, 0.00085);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.10, 0.0010);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.15, 0.0014);

	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.20, 0.0020);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.25, 0.0038);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.30, 0.006);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.35, 0.010);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.40, 0.040);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.45, 0.095);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.47, 0.100);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.52, 0.05);

	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.54, 0.037);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.62, 0.022);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.66, 0.021);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.75, 0.018);

	g_high_t_in->SetPoint(g_high_t_in->GetN(), 0.85, 0.027);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 1.00, 0.070);
	g_high_t_in->SetPoint(g_high_t_in->GetN(), 1.10, 0.070);

	// make final correction graph
	TGraph *g_corr = new TGraph();
	g_corr->SetName("g_corr");

	for (double mt = 0.0; mt < 0.2; mt += 0.001)
	{
		g_corr->SetPoint(g_corr->GetN(), mt, g_high_t_in->Eval(mt, NULL, ""));
	}

	for (double mt = 0.2; mt <= 1.1; mt += 0.01)
	{
		g_corr->SetPoint(g_corr->GetN(), mt, g_high_t_in->Eval(mt, NULL, "S"));
	}

	// make comparison plot
	h_bb_bt->SetLineColor(2);
	h_tt_bt->SetLineColor(4);
	h_bb_tb->SetLineColor(6);
	h_tt_tb->SetLineColor(8);

	TCanvas *c = new TCanvas();
	c->SetName("cmp");

	h_bb_bt->Draw("");
	h_tt_bt->Draw("same");
	h_bb_tb->Draw("same");
	h_tt_tb->Draw("same");

	g_high_t_in->Draw("l");
	g_corr->Draw("l");

	c->Write();

	g_corr->Write();
	
	// clean up
	delete f_out;

	return 0;
}
