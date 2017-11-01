#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"

#include <vector>
#include <cmath>

#include "../stat.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

vector<TGraphErrors *> fit_data;
vector<TGraph *> fit_graphs;

void AddFit(const string &fn)
{
	TFile *f = new TFile(fn.c_str());
	fit_data.push_back( (TGraphErrors *) f->Get("data_diff, final") );
	fit_graphs.push_back( (TGraphErrors *) f->Get("ff_T, final") );
}

//----------------------------------------------------------------------------------------------------

string Format(double v, bool sign, int N_bef, int N_aft, int N_sig)
{
	string output = "";

	if (sign)
	{
		if (v < 0)
			output += "-";
		else
			output += "+";
	}

	signed int order = floor(log10(fabs(v)));

	signed int pos_left = order;
	signed int pos_right = order - N_sig + 1;
	signed int pos_start = N_bef - 1;
	signed int pos_end = -N_aft;

	signed int pos_round = max(pos_right, pos_end);

	int v_rounded = floor(fabs(v) * pow(10, -pos_round) + 0.5);
	char buf[20];
	sprintf(buf, "%i", v_rounded);

	/*
	printf("\n");
	printf("v = %E\n", v);
	printf("buf = %s\n", buf);
	printf("order = %i\n", order);
	printf("pos_left = %i\n", pos_left);
	printf("pos_right = %i\n", pos_right);
	printf("pos_start = %i\n", pos_start);
	printf("pos_end = %i\n", pos_end);
	*/

	string spaceStr = "\\S";

	for (signed int pos = pos_start; pos >= pos_end; pos--)
	{
		//printf("pos = %i\n", pos);

		// add decimal point
		if (pos == -1)
			output += ".";

		if (pos > pos_left)
		{
			if (pos > 0)
				output += spaceStr;
			else
				output += "0";
		}

		if (pos < pos_right)
		{
			if (pos < 0)
				output += spaceStr;
			else
				output += "0";
		}

		if (pos >= pos_right && pos <= pos_left)
			output += buf[pos_left - pos];

		//printf("\t<%s>\n", output.c_str());
	}

	return output;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

int main()
{
	string dataset = "merged";
	string diagonal = "combined";
	string binning = "ob-2-10-0.05";

	// get data
	TFile *f_data_in = new TFile("../DS-merged/merged.root");
	TH1D *h_data = (TH1D *) f_data_in->Get((binning + "/" + dataset + "/" + diagonal + "/h_dsdt").c_str());
	printf("h_data = %p\n", h_data);

	// get uncertainty
	string f_unc_name = "../systematics/DS2b/matrix_numerical_integration.root";
	TFile *f_unc_in = new TFile(f_unc_name.c_str());

	TH1D *h_unc_all = (TH1D *) f_unc_in->Get(("matrices/all/"+diagonal+"/"+binning+"/h_stddev").c_str());
	
	TH1D *h_unc_all_anal = (TH1D *) f_unc_in->Get(("matrices/all-anal/"+diagonal+"/"+binning+"/h_stddev").c_str());
	
	printf("h_unc_all = %p\n", h_unc_all);
	printf("h_unc_all_anal = %p\n", h_unc_all_anal);
	
	TGraph *g_unc_vec_norm = (TGraph *) f_unc_in->Get("contributions/norm/g_eff_comb1");
	TGraph *g_unc_vec_opt1 = (TGraph *) f_unc_in->Get("contributions/opt-scale-m1/g_eff_comb1");
	TGraph *g_unc_vec_opt2 = (TGraph *) f_unc_in->Get("contributions/opt-scale-m2/g_eff_comb1");
	TGraph *g_unc_vec_bmom = (TGraph *) f_unc_in->Get("contributions/beam-mom/g_eff_comb1");
	TGraph *g_unc_vec_al_shx = (TGraph *) f_unc_in->Get("contributions/alig-sh-thx/g_eff_comb1");
	//TGraph *g_unc_vec_al_shy_R = (TGraph *) ((TDirectory *) f_unc_in->Get("contributions/alig-sh-thy:D+0,R+1"))->Get("g_eff_comb1");
	//TGraph *g_unc_vec_accor_si = (TGraph *) f_unc_in->Get("contributions/acc-corr-sigma-unc/g_eff_comb1");

	// fits
	string fit_base = "../coulomb_analysis/data/" + binning + "/" + dataset + "/" + diagonal + "/";
	AddFit(fit_base + "fit:1,KL,con,chisq,,st+sy/dsdt.root");
	AddFit(fit_base + "fit:2,KL,con,chisq,,st+sy/dsdt.root");
	AddFit(fit_base + "fit:3,KL,con,chisq,,st+sy/dsdt.root");

	// TODO
	TGraph *ref_fit = fit_graphs.back();

	// output
	TFile *f_out = new TFile("tabulate.root", "recreate");

	TGraphErrors *g_data = new TGraphErrors(); g_data->SetName("g_data");
	TGraph *g_unc_all_pl = new TGraph(); g_unc_all_pl->SetName("g_unc_all_pl"); g_unc_all_pl->SetLineColor(2);
	TGraph *g_unc_all_mi = new TGraph(); g_unc_all_mi->SetName("g_unc_all_mi"); g_unc_all_mi->SetLineColor(2);
	TGraph *g_unc_anal_all_pl = new TGraph(); g_unc_anal_all_pl->SetName("g_unc_anal_all_pl"); g_unc_anal_all_pl->SetLineColor(4);
	TGraph *g_unc_anal_all_mi = new TGraph(); g_unc_anal_all_mi->SetName("g_unc_anal_all_mi"); g_unc_anal_all_mi->SetLineColor(4);
	TGraph *g_unc_anal_lead_pl = new TGraph(); g_unc_anal_lead_pl->SetName("g_unc_anal_lead_pl"); g_unc_anal_lead_pl->SetLineColor(8);
	TGraph *g_unc_anal_lead_mi = new TGraph(); g_unc_anal_lead_mi->SetName("g_unc_anal_lead_mi"); g_unc_anal_lead_mi->SetLineColor(8);

	TGraph *g_band_cen = new TGraph(); g_band_cen->SetName("g_band_cen");
	TGraph *g_unc_all = new TGraph(); g_unc_all->SetName("g_unc_all");
	TGraph *g_unc_anal_all = new TGraph(); g_unc_anal_all->SetName("g_unc_anal_all");

	TGraphErrors *g_rel_data = new TGraphErrors(); g_rel_data->SetName("g_rel_data"); g_rel_data->SetMarkerStyle(20);
	TGraph *g_rel_unc_all_pl = new TGraph(); g_rel_unc_all_pl->SetName("g_rel_unc_all_pl"); g_rel_unc_all_pl->SetLineColor(2);
	TGraph *g_rel_unc_all_ce = new TGraph(); g_rel_unc_all_ce->SetName("g_rel_unc_all_ce"); g_rel_unc_all_ce->SetLineColor(2); g_rel_unc_all_ce->SetLineStyle(2);
	TGraph *g_rel_unc_all_mi = new TGraph(); g_rel_unc_all_mi->SetName("g_rel_unc_all_mi"); g_rel_unc_all_mi->SetLineColor(2);
	TGraph *g_rel_unc_anal_all_pl = new TGraph(); g_rel_unc_anal_all_pl->SetName("g_rel_unc_anal_all_pl"); g_rel_unc_anal_all_pl->SetLineColor(4);
	TGraph *g_rel_unc_anal_all_mi = new TGraph(); g_rel_unc_anal_all_mi->SetName("g_rel_unc_anal_all_mi"); g_rel_unc_anal_all_mi->SetLineColor(4);
	TGraph *g_rel_unc_anal_lead_pl = new TGraph(); g_rel_unc_anal_lead_pl->SetName("g_rel_unc_anal_lead_pl"); g_rel_unc_anal_lead_pl->SetLineColor(8);
	TGraph *g_rel_unc_anal_lead_mi = new TGraph(); g_rel_unc_anal_lead_mi->SetName("g_rel_unc_anal_lead_mi"); g_rel_unc_anal_lead_mi->SetLineColor(8);

	TGraph *g_rel0_unc_all_pl = new TGraph(); g_rel0_unc_all_pl->SetName("g_rel0_unc_all_pl"); g_rel0_unc_all_pl->SetLineColor(2);
	TGraph *g_rel0_unc_all_mi = new TGraph(); g_rel0_unc_all_mi->SetName("g_rel0_unc_all_mi"); g_rel0_unc_all_mi->SetLineColor(2);
	TGraph *g_rel0_unc_anal_all_pl = new TGraph(); g_rel0_unc_anal_all_pl->SetName("g_rel0_unc_anal_all_pl"); g_rel0_unc_anal_all_pl->SetLineColor(4);
	TGraph *g_rel0_unc_anal_all_mi = new TGraph(); g_rel0_unc_anal_all_mi->SetName("g_rel0_unc_anal_all_mi"); g_rel0_unc_anal_all_mi->SetLineColor(4);
	TGraph *g_rel0_unc_anal_lead_pl = new TGraph(); g_rel0_unc_anal_lead_pl->SetName("g_rel0_unc_anal_lead_pl"); g_rel0_unc_anal_lead_pl->SetLineColor(8);
	TGraph *g_rel0_unc_anal_lead_mi = new TGraph(); g_rel0_unc_anal_lead_mi->SetName("g_rel0_unc_anal_lead_mi"); g_rel0_unc_anal_lead_mi->SetLineColor(8);

	// print out
	int bi0 = 0;
	bool bi0_set = false;

	for (int bi = 1; bi <= h_data->GetNbinsX(); bi++)
	{
		double b_left = h_data->GetBinLowEdge(bi);
		double b_right = b_left + h_data->GetBinWidth(bi);

		double v = h_data->GetBinContent(bi);
		double v_unc_stat = h_data->GetBinError(bi);

		if (v == 0.)
			continue;

		if (!bi0_set)
		{
			bi0 = bi;
			bi0_set = true;
			printf("bi0 = %i\n", bi0);
		}

		if (b_left >= 0.2)
			continue;

		// bin representative point statistics
		// TODO
		/*
		int gi = bi - bi0;
		Stat st_x_rep(1);
		for (unsigned int fi = 0; fi < fit_data.size(); fi++)
		{
			double x=0, y;
			fit_data[fi]->GetPoint(gi, x, y);
			st_x_rep.Fill(x);
		}

		double b_rep_mean = st_x_rep.GetMean(0);
		double b_rep_mean_unc = st_x_rep.GetMeanUnc(0);

		if (b_rep_mean < b_left || b_rep_mean > b_right)
		{
			printf("ERROR: bad synchronisation betweeen data histogram and fit graph, bin %u.\n", bi);
		}
		*/
		double b_rep_mean = h_data->GetBinCenter(bi);
		double b_rep_mean_unc = 0.;

		// reference bin content
		// TODO
		//double v_ref = ref_fit->Eval(b_rep_mean);
		double v_ref = 0.;

		// systematics
		// TODO
		/*
		double v_unc_syst_all = h_unc_all->GetBinContent(bi) * v_ref;
		double v_unc_syst_all_anal = h_unc_all_anal->GetBinContent(bi) * v_ref;

		double v_unc_syst_norm = g_unc_vec_norm->Eval(b_rep_mean) * v_ref;
		double v_unc_syst_opt1 = g_unc_vec_opt1->Eval(b_rep_mean) * v_ref;
		double v_unc_syst_opt2 = g_unc_vec_opt2->Eval(b_rep_mean) * v_ref;
		double v_unc_syst_bmom = g_unc_vec_bmom->Eval(b_rep_mean) * v_ref;
		double v_unc_syst_al_shx = g_unc_vec_al_shx->Eval(b_rep_mean) * v_ref;
		double v_unc_syst_al_shy_R = g_unc_vec_al_shy_R->Eval(b_rep_mean) * v_ref;
		*/

		double v_unc_syst_all = 0.;
		double v_unc_syst_all_anal = 0.;
		double v_unc_syst_norm = 0.;
		double v_unc_syst_opt1 = 0.;
		double v_unc_syst_opt2 = 0.;
		double v_unc_syst_bmom = 0.;
		double v_unc_syst_al_shx = 0.;
		double v_unc_syst_al_shy_R = 0.;

		double v_unc_syst_anal_lead = sqrt(
			v_unc_syst_opt1*v_unc_syst_opt1
			+ v_unc_syst_opt2*v_unc_syst_opt2
			+ v_unc_syst_bmom*v_unc_syst_bmom
			+ v_unc_syst_al_shx*v_unc_syst_al_shx
			+ v_unc_syst_al_shy_R * v_unc_syst_al_shy_R
		);

		// print formatted line
		// short
		printf("%.4E, %.4E | %.4E, %.4E\n",
			b_left, b_right,
			v, v_unc_stat
		);

		// full
		/*
		printf("%.6f, %.6f, %.6f | %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.4f\n",
			b_left, b_right, b_rep_mean,
			v, v_unc_stat,
			v_unc_syst_all, v_unc_syst_norm, v_unc_syst_opt1, v_unc_syst_opt2, v_unc_syst_bmom, v_unc_syst_al_shx, v_unc_syst_al_shy_R
		);
		*/

		// TeX
		// & $%8.3f$ & $%+5.3f$ & $%+5.3f$ & $%+5.3f$

		/*
		printf("$%.5f$ & $%.5f$ & $%.5f$ & $%7.3f$ & $%5.3f$ & $%6.3f$ & $%8.3f$ & $%+5.3f$ & $%+5.3f$ & $%+5.3f$ & $%+5.3f$ & $%+5.3f$ \\cr\n",
			b_left, b_right, b_rep_mean,
			v, v_unc_stat, v_unc_syst_all,
			v_unc_syst_norm, v_unc_syst_opt1, v_unc_syst_opt2, v_unc_syst_bmom, v_unc_syst_al_shx, v_unc_syst_al_shy_R
		);
		*/

		/*
		printf("$%s$ & $%s$ & $%s$ & $%s$ & $%s$ & $%s$ & $%s$ & $%s$ & $%s$ & $%s$ & $%s$ & $%s$ \\cr\n",
			Format(b_left, false, 1, 6, 4).c_str(),
			Format(b_right, false, 1, 6, 4).c_str(),
			Format(b_rep_mean, false, 1, 6, 4).c_str(),

			Format(v, false, 3, 2, 5).c_str(),
			Format(v_unc_stat, false, 2, 2, 3).c_str(),
			Format(v_unc_syst_all, false, 2, 2, 3).c_str(),

			Format(v_unc_syst_norm, true, 2, 2, 3).c_str(),
			Format(v_unc_syst_opt1, true, 1, 3, 3).c_str(),
			Format(v_unc_syst_opt2, true, 1, 3, 3).c_str(),
			Format(v_unc_syst_bmom, true, 1, 3, 3).c_str(),
			Format(v_unc_syst_al_shx, true, 1, 3, 3).c_str(),
			Format(v_unc_syst_al_shy_R, true, 2, 3, 3).c_str()
		);
		*/

		// fill output
		int idx = g_data->GetN();

		g_data->SetPoint(idx, b_rep_mean, v);
		g_data->SetPointError(idx, b_rep_mean_unc, v_unc_stat);

		g_unc_all_pl->SetPoint(idx, b_rep_mean, v_ref + v_unc_syst_all);
		g_unc_all_mi->SetPoint(idx, b_rep_mean, v_ref - v_unc_syst_all);

		g_unc_anal_all_pl->SetPoint(idx, b_rep_mean, v_ref + v_unc_syst_all_anal);
		g_unc_anal_all_mi->SetPoint(idx, b_rep_mean, v_ref - v_unc_syst_all_anal);

		g_unc_anal_lead_pl->SetPoint(idx, b_rep_mean, v_ref + v_unc_syst_anal_lead);
		g_unc_anal_lead_mi->SetPoint(idx, b_rep_mean, v_ref - v_unc_syst_anal_lead);
		
		//----------

		g_band_cen->SetPoint(idx, b_rep_mean, v_ref);
		g_unc_all->SetPoint(idx, b_rep_mean, v_unc_syst_all);
		g_unc_anal_all->SetPoint(idx, b_rep_mean, v_unc_syst_all_anal);

		//----------

		double A_ref = 527.1, B_ref = 19.39;
		double v_rel_ref = A_ref * exp(-B_ref * b_rep_mean);
		
		g_rel_data->SetPoint(idx, b_rep_mean, v / v_rel_ref - 1.);
		g_rel_data->SetPointError(idx, b_rep_mean_unc, v_unc_stat / v_rel_ref);

		g_rel_unc_all_pl->SetPoint(idx, b_rep_mean, (v_ref + v_unc_syst_all) / v_rel_ref - 1.);
		g_rel_unc_all_ce->SetPoint(idx, b_rep_mean, v_ref / v_rel_ref - 1.);
		g_rel_unc_all_mi->SetPoint(idx, b_rep_mean, (v_ref - v_unc_syst_all) / v_rel_ref - 1.);

		g_rel_unc_anal_all_pl->SetPoint(idx, b_rep_mean, (v_ref + v_unc_syst_all_anal) / v_rel_ref - 1.);
		g_rel_unc_anal_all_mi->SetPoint(idx, b_rep_mean, (v_ref - v_unc_syst_all_anal) / v_rel_ref - 1.);

		g_rel_unc_anal_lead_pl->SetPoint(idx, b_rep_mean, (v_ref + v_unc_syst_anal_lead) / v_rel_ref - 1.);
		g_rel_unc_anal_lead_mi->SetPoint(idx, b_rep_mean, (v_ref - v_unc_syst_anal_lead) / v_rel_ref - 1.);

		//----------

		g_rel0_unc_all_pl->SetPoint(idx, b_rep_mean, v_unc_syst_all / v_rel_ref);
		g_rel0_unc_all_mi->SetPoint(idx, b_rep_mean, v_unc_syst_all / v_rel_ref);

		g_rel0_unc_anal_all_pl->SetPoint(idx, b_rep_mean, v_unc_syst_all_anal / v_rel_ref);
		g_rel0_unc_anal_all_mi->SetPoint(idx, b_rep_mean, v_unc_syst_all_anal / v_rel_ref);

		g_rel0_unc_anal_lead_pl->SetPoint(idx, b_rep_mean, v_unc_syst_anal_lead / v_rel_ref);
		g_rel0_unc_anal_lead_mi->SetPoint(idx, b_rep_mean, v_unc_syst_anal_lead / v_rel_ref);
	}

	g_data->Write();
	g_unc_all_pl->Write();
	g_unc_all_mi->Write();
	g_unc_anal_all_pl->Write();
	g_unc_anal_all_mi->Write();
	g_unc_anal_lead_pl->Write();
	g_unc_anal_lead_mi->Write();

	g_band_cen->Write();
	g_unc_all->Write();
	g_unc_anal_all->Write();

	g_rel_data->Write();
	g_rel_unc_all_pl->Write();
	g_rel_unc_all_ce->Write();
	g_rel_unc_all_mi->Write();
	g_rel_unc_anal_all_pl->Write();
	g_rel_unc_anal_all_mi->Write();
	g_rel_unc_anal_lead_pl->Write();
	g_rel_unc_anal_lead_mi->Write();

	TCanvas *c = new TCanvas("c");

	g_data->Draw("ap");
	g_unc_all_pl->Draw("l");
	g_unc_all_mi->Draw("l");
	g_unc_anal_all_pl->Draw("l");
	g_unc_anal_all_mi->Draw("l");
	g_unc_anal_lead_pl->Draw("l");
	g_unc_anal_lead_mi->Draw("l");

	c->Write();

	TCanvas *c_rel = new TCanvas("c_rel");
	TH2D *h_frame = new TH2D("h_frame", "", 100, 0., 0.2, 100, -0.10, +0.10);

	h_frame->Draw();
	g_rel_data->Draw("p");
	g_rel_unc_all_pl->Draw("l");
	g_rel_unc_all_ce->Draw("l");
	g_rel_unc_all_mi->Draw("l");
	g_rel_unc_anal_all_pl->Draw("l");
	g_rel_unc_anal_all_mi->Draw("l");
	g_rel_unc_anal_lead_pl->Draw("l");
	g_rel_unc_anal_lead_mi->Draw("l");

	c_rel->Write();

	TCanvas *c_rel0 = new TCanvas("c_rel0");
	h_frame = new TH2D("h_frame0", "", 100, 0., 0.2, 100, -0.20, +0.20);

	h_frame->Draw();
	g_rel0_unc_all_pl->Draw("l");
	g_rel0_unc_all_mi->Draw("l");
	g_rel0_unc_anal_all_pl->Draw("l");
	g_rel0_unc_anal_all_mi->Draw("l");
	g_rel0_unc_anal_lead_pl->Draw("l");
	g_rel0_unc_anal_lead_mi->Draw("l");

	c_rel0->Write();

	delete f_out;

	return 0;
}
