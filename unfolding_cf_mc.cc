#include "TGraph.h"
#include "TFile.h"
#include "TH1D.h"
#include "TSpline.h"

#include <string>
#include <map>
#include <cstdio>

#include "common_definitions.h"
#include "common_algorithms.h"
#include "parameters.h"
#include "common.h"
#include "AcceptanceCalculator.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

TSpline* BuildSpline(TGraph *g)
{
	TSpline3 *s = new TSpline3("", g->GetX(), g->GetY(), g->GetN());
	s->SetName(g->GetName());
	return s;
}

//----------------------------------------------------------------------------------------------------

struct Model
{
	string label;
	string file;
	string object;
};

//----------------------------------------------------------------------------------------------------

struct PlotGroup
{
	TH1D *h_t_tr = NULL, *h_t_sm = NULL;

	void Init(unsigned int Nbins, double *binEdges)
	{
		h_t_tr = new TH1D("", ";t", Nbins, binEdges); h_t_tr->Sumw2();
		h_t_sm = new TH1D("", ";t", Nbins, binEdges); h_t_sm->Sumw2();
	}

	void Write() const
	{
		h_t_tr->Scale(1., "width");
		h_t_tr->SetLineColor(4);
		h_t_tr->Write("h_t_tr");

		h_t_sm->Scale(1., "width");
		h_t_sm->SetLineColor(2);
		h_t_sm->Write("h_t_sm");

		TH1D *h_corr = new TH1D(*h_t_tr);
		for (int bi = 1; bi <= h_corr->GetNbinsX(); ++bi)
		{
			double v_tr = h_t_tr->GetBinContent(bi);
			double u_tr = h_t_tr->GetBinError(bi);

			double v_sm = h_t_sm->GetBinContent(bi);
			double u_sm = h_t_sm->GetBinError(bi);

			double corr = (v_sm > 0.) ? v_tr / v_sm : 0.;
			double corr_u = (v_sm > 0. && v_tr > 0.) ? corr * sqrt( pow(u_tr/v_tr, 2.) + pow(u_sm/v_sm, 2.) ) : 0.;

			h_corr->SetBinContent(bi, corr);
			h_corr->SetBinError(bi, corr_u);
		}

		h_corr->SetLineColor(8);
		h_corr->Write("h_corr");
	}
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

	// settings
	unsigned int N_ev = (unsigned int) 4E9;
	unsigned int seed = 1;

	gRandom->SetSeed(seed);
	
	// binnings
	vector<string> binnings;
	//binnings.push_back("ob-1-20-0.05");
	//binnings.push_back("ob-2-10-0.05");
	binnings.push_back("ob-3-5-0.05");

	// models
	vector<Model> models = {
		{ "fit2-2", "/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/first_fits/2/do_fit.root", "variant 2/g_dsdt_CH" },
	};

	// print info
	printf("\n");
	printf("N_ev = %u = %.1E\n", N_ev, (double) N_ev);
	printf("seed = %u\n", seed);
	printf("\n");
	printf("------------------------------ environment ------------------------------\n");
	env.Print();
	printf("\n");
	printf("------------------------------- analysis --------------------------------\n");
	anal.Print();
	printf("\n");

	// initialize acceptance calculator
	AcceptanceCalculator accCalc;
	accCalc.Init(th_y_sign, anal);

	// load input
	vector<TSpline *> modelSplines;
	for (const auto model : models)
	{
		TFile *f_in = TFile::Open(model.file.c_str());
		TGraph *g_dsdt = (TGraph *) f_in->Get(model.object.c_str());

		modelSplines.push_back(BuildSpline(g_dsdt));
	}

	// book histograms
	vector<vector<PlotGroup>> plots;
	for (unsigned int mi = 0; mi < models.size(); ++mi)
	{
		vector<PlotGroup> bv;

		for (unsigned int bi = 0; bi < binnings.size(); ++bi)
		{
			unsigned int N_bins;
			double *bin_edges;
			BuildBinning(anal, binnings[bi], bin_edges, N_bins, false);

			PlotGroup pg;
			pg.Init(N_bins, bin_edges);

			bv.push_back(pg);
		}

		plots.push_back(bv);
	}

	// prepare output
	string fn_out = string("unfolding_cf_mc_") + argv[1] + ".root";
	TFile *f_out = new TFile(fn_out.c_str(), "recreate");
	if (f_out->IsZombie())
	{
		printf("ERROR: can't open file `%s' for writing.\n", fn_out.c_str());
		return 3;
	}

	// generator distribution
	double v1 = 100., m1 = 0.05;
	double v2 = 10, m2 = 0.4;
	double v3 = 1., m3 = anal.t_max_full;

	double norm = v1 * (m1-0.) + v2 * (m2-m1) + v3 * (m3-m2);

	double f1 = v1 * (m1-0.) / norm;
	double f2 = f1 + v2 * (m2-m1) / norm;
	double f3 = 1.;

	// event loop
	for (unsigned int evi = 0; evi < N_ev; ++evi)
	{
		double k = gRandom->Rndm();
		double t = 0., sw = 0.;
		if (k <= f1) { t = k/f1 * m1; sw = v1; }
		if (f1 < k && k <= f2) { t = m1 + (k-f1)/(f2-f1)*(m2-m1); sw = v2; }
		if (f2 < k && k <= f3) { t = m2 + (k-f2)/(f3-f2)*(m3-m2); sw = v3; }

		// generate true event
		Kinematics k_tr;
		k_tr.t = t;
		k_tr.phi = gRandom->Rndm() * M_PI * th_y_sign;

		k_tr.TPhiToThetas(env);

		// simulate smearing
		Kinematics k_sm = k_tr;
		k_sm.th_x += gRandom->Gaus() * anal.si_th_x_2arm;
		k_sm.th_y += gRandom->Gaus() * anal.si_th_y_2arm;

		k_sm.ThetasToTPhi(env);

		// phi acceptance correction
		bool skip = accCalc.PhiComponentCut(k_sm.th_x, k_sm.th_y);
		double phiFactor = 0.;
		if (!skip)
			phiFactor = accCalc.PhiFactor(k_sm.th) / 2.;

		// calculate event weights
		vector<double> ws;
		for (const auto spline : modelSplines)
			ws.push_back(spline->Eval(k_tr.t) / sw);

		// fill true event
		for (unsigned int mi = 0; mi < models.size(); ++mi)
		{
			for (unsigned int bi = 0; bi < binnings.size(); ++bi)
			{
				plots[mi][bi].h_t_tr->Fill(k_tr.t, ws[mi]);

				if (!skip)
					plots[mi][bi].h_t_sm->Fill(k_sm.t, ws[mi] * phiFactor);
			}
		}
	}

	// save plots
	for (unsigned int mi = 0; mi < models.size(); ++mi)
	{
		TDirectory *d_model = f_out->mkdir(models[mi].label.c_str());

		for (unsigned int bi = 0; bi < binnings.size(); ++bi)
		{
			TDirectory *d_binning = d_model->mkdir(binnings[bi].c_str());
			gDirectory = d_binning;

			plots[mi][bi].Write();
		}
	}

	// clean up
	delete f_out;

	return 0;
}
