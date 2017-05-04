#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TMatrixDSym.h"
#include "TSpline.h"

#include <string>

#include "../common_algorithms.h"


using namespace std;

//----------------------------------------------------------------------------------------------------

enum CorrelationType { coNo, coFull, coAntiFull };

struct Mode
{
	string tag;
	string fileName;
	string objName;
	double ref;
	CorrelationType corr;

	TGraph *g_input_45b, *g_input_45t;

	TGraph *g_eff_45b, *g_eff_45t, *g_eff_comb1, *g_eff_comb2;
};

vector<Mode> modes;

//----------------------------------------------------------------------------------------------------

void AddMode(string _t, string _fn, string _on, double _ref, CorrelationType _c)
{
	Mode m;
	m.tag = _t;
	m.fileName = _fn;
	m.objName = _on;
	m.ref = _ref;
	m.corr = _c;

	m.g_eff_45b = NULL;
	m.g_eff_45t = NULL;
	m.g_eff_comb1 = NULL;
	m.g_eff_comb2 = NULL;

	modes.push_back(m);
}

//----------------------------------------------------------------------------------------------------

TSpline* BuildSpline(TGraph *g)
{
	TSpline3 *s = new TSpline3("", g->GetX(), g->GetY(), g->GetN());
	s->SetName(g->GetName());
	return s;
}

//----------------------------------------------------------------------------------------------------

void MakeMatrix(const vector<string> &contributions, TDirectory *topDir, const string &label)
{
	printf(">> MakeMatrix(%s)\n", label.c_str());

	// select binnings and diagonals
	vector<string> binnings;
	binnings.push_back("ob-1-20-0.05");
	binnings.push_back("ob-2-10-0.05");
	binnings.push_back("ob-3-5-0.05");

	vector<string> diagonals;
	diagonals.push_back("45b_56t");
	diagonals.push_back("45t_56b");
	diagonals.push_back("combined");

	// get selected modes
	vector<Mode *> sel_modes;
	for (unsigned int ci = 0; ci < contributions.size(); ci++)
	{
		Mode *m = NULL;
		for (unsigned int mi = 0; mi < modes.size(); mi++)
		{
			if (modes[mi].tag.compare(contributions[ci]) == 0)
			{
				m = & modes[mi];
				break;
			}
		}

		if (m == NULL)
		{
			printf("ERROR: mode `%s' doesn't exist.\n", contributions[ci].c_str());
			return;
		}

		sel_modes.push_back(m);
	}
	printf("\tselected modes: %lu\n", sel_modes.size());

	// make subdirectory
	TDirectory *groupDir = topDir->mkdir(label.c_str());

	// process all diagonals
	for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
	{
		printf("\t%s\n", diagonals[dgni].c_str());

		TDirectory *dgnDir = groupDir->mkdir(diagonals[dgni].c_str());

		// list of contributing graphs
		vector<TGraph *> graphs;

		for (unsigned int mi = 0; mi < sel_modes.size(); mi++)
		{
			if (diagonals[dgni].compare("45b_56t") == 0)
				graphs.push_back(sel_modes[mi]->g_eff_45b);

			if (diagonals[dgni].compare("45t_56b") == 0)
				graphs.push_back(sel_modes[mi]->g_eff_45t);

			if (diagonals[dgni].compare("combined") == 0)
			{
				graphs.push_back(sel_modes[mi]->g_eff_comb1);
				if (sel_modes[mi]->g_eff_comb2 != NULL)
					graphs.push_back(sel_modes[mi]->g_eff_comb2);
			}
		}
		printf("\t\tgraphs: %lu\n", graphs.size());

		if (graphs.size() < 1)
			continue;

		// build splines
		vector<TSpline *> splines;
		for (TGraph *g : graphs)
			splines.push_back(BuildSpline(g));
			
		// make matrix for each binning
		for (unsigned int bni = 0; bni < binnings.size(); bni++)
		{
			printf("\t\t%s\n", binnings[bni].c_str());

			// build binning
			Analysis anal;
			anal.t_min = 8E-4; anal.t_max = 1.0;
			anal.t_min_full = 0.; anal.t_max_full = 1.1;

			unsigned int bins;
			double *bin_edges;
			BuildBinning(anal, binnings[bni], bin_edges, bins);

			// make binning subdirectory
			TDirectory *binDir = dgnDir->mkdir(binnings[bni].c_str());
			gDirectory = binDir;

			// build matrix
			TH1D *h_stddev = new TH1D("h_stddev", ";|t|", bins, bin_edges);

			vector<vector<double>> conPerBin;
			for (unsigned int gi = 0; gi < graphs.size(); gi++)
			{
				vector<double> temp;
				for (unsigned int i = 0; i < bins; i++)
				{
					const int bi = i + 1;
					const double l = h_stddev->GetBinLowEdge(bi);
					const double r = l + h_stddev->GetBinWidth(bi);

					const unsigned int n_div = 100.;	// number of bin divisions
					double S = 0.;
					for (unsigned int i_div = 0; i_div < n_div; i_div++)
					{
						const double t = l + (0.5 + i_div) * (r - l) / n_div;
						S += splines[gi]->Eval(t);
					}
					const double avg = S / n_div;

					temp.push_back(avg);
				}

				conPerBin.push_back(temp);
			}

			TMatrixDSym cov_mat(bins);
			for (unsigned int i = 0; i < bins; i++)
			{
				for (unsigned int j = 0; j < bins; j++)
				{
					double S = 0.;
					for (unsigned int gi = 0; gi < graphs.size(); gi++)
						S += conPerBin[gi][i] * conPerBin[gi][j];
					cov_mat(i, j) = S;
				}
			}

			cov_mat.Write("cov_mat");

			// build histograms
			TH2D *h_corr_mat = new TH2D("h_corr_mat", ";|t|;|t|", bins, bin_edges, bins, bin_edges);
			for (unsigned int i = 0; i < bins; i++)
			{
				for (unsigned int j = 0; j < bins; j++)
				{
					int bi = i + 1;
					int bj = j + 1;

					double rho = cov_mat(i, j) / sqrt(cov_mat(i, i) * cov_mat(j, j));

					h_corr_mat->SetBinContent(bi, bj, rho);

					if (i == j)
					{
						double stddev = sqrt(cov_mat(i, j));
						h_stddev->SetBinContent(bi, stddev);
					}
				}
			}

			h_stddev->Write();
			h_corr_mat->Write();
		}

		// make envelope
		gDirectory = dgnDir;

		TGraph *g_envelope = new TGraph();
		g_envelope->SetName("g_envelope");

		TGraph *g_ref = graphs[0];

		for (int idx = 0; idx < g_ref->GetN(); ++idx)
		{
			double t, dummy;
			g_ref->GetPoint(idx, t, dummy);

			double S2 = 0.;

			for (auto s : splines)
			{
				double v = s->Eval(t);
				S2 += v*v;
			}

			g_envelope->SetPoint(g_envelope->GetN(), t, sqrt(S2));
		}

		g_envelope->Write();

		// release splines
		for (auto s : splines)
			delete s;
	}
}


//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: matrix_numerical_integration <input directory> <output file name>\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	if (argc != 3)
	{
		printf("ERROR: wrong number of parameters\n");
		PrintUsage();
		return 1;
	}

	// get command-line arguments
	string inputDir = argv[1];
	string outputFileName = argv[2];

	string fn_ni_45b = inputDir + "/numerical_integration_full_45b_56t.root";
	string fn_ni_45t = inputDir + "/numerical_integration_full_45t_56b.root";

	string t_dist_type = "fit2-2";

	// analyses cuts
	double t_min_45b = 8E-4;
	double t_min_45t = 8E-4;

	// get input
	TFile *f_ni_45b = TFile::Open(fn_ni_45b.c_str());
	TFile *f_ni_45t = TFile::Open(fn_ni_45t.c_str());

	if (!f_ni_45b || !f_ni_45t)
	{
		printf("ERROR: can't open input files.\n");
		return 1;
	}

	// get graphs of cross-section with and without acceptance
	// TODO
	/*
	TGraph *g_acc_dsdt_45b = (TGraph*) f_ni_45b->Get(("none/" + t_dist_type + "/g_h_obs").c_str());
	TGraph *g_acc_dsdt_45t = (TGraph*) f_ni_45t->Get(("none/" + t_dist_type + "/g_h_obs").c_str());
	TGraph *g_dsdt_45b = (TGraph*) f_ni_45b->Get(("none/" + t_dist_type + "/g_h_p").c_str());
	TGraph *g_dsdt_45t = (TGraph*) f_ni_45t->Get(("none/" + t_dist_type + "/g_h_p").c_str());

	if (!g_acc_dsdt_45b || !g_acc_dsdt_45t || !g_dsdt_45b || !g_dsdt_45t)
	{
		printf("ERROR: can't load observed cross-section graphs.\n");
		return 1;
	}
	*/

	// prepare output
	TFile *f_out = new TFile(outputFileName.c_str(), "recreate");

	// ---------- give source information for all modes ----------

	AddMode("alig-sh-thx", "s", "<TDIST>/alig-sh-thx/g_eff", 1., coFull);
	AddMode("alig-sh-thy", "s", "<TDIST>/alig-sh-thy/g_eff", 1., coFull);
	
	AddMode("thx-thy-tilt", "s", "<TDIST>/thx-thy-tilt/g_eff", 1., coFull);
	
	AddMode("opt-m1", "s", "<TDIST>/opt-m1/g_eff", 1., coFull);
	AddMode("opt-m2", "s", "<TDIST>/opt-m2/g_eff", 1., coFull);

	
	AddMode("acc-corr-sigma-unc-thx", "s", "<TDIST>/acc-corr-sigma-unc-thx/g_eff", 1., coFull);
	AddMode("acc-corr-sigma-unc-thy", "s", "<TDIST>/acc-corr-sigma-unc-thy/g_eff", 1., coFull);

	// TODO
	//AddMode("acc-corr-sigma-asym", "s", "x+0,y+0/<TDIST>/sigma asymmetry/systematic effect", 1., coFull);
	//AddMode("acc-corr-non-gauss", "s", "x+0,y+0/<TDIST>/non-gauss/systematic effect", 1., coFull);

	AddMode("eff-slp", "s", "<TDIST>/eff-slp/g_eff", 1., coNo);

	AddMode("beam-mom", "s", "<TDIST>/beam-mom/g_eff", 1., coFull);
	
	// TODO
	/*
	AddMode("unsm-sigma-x", "", "unsmearing correction/<TDIST>/unsm_corr_unc_th_x", 0., coFull);
	AddMode("unsm-sigma-y", "", "unsmearing correction/<TDIST>/unsm_corr_unc_th_y", 0., coFull);
	AddMode("unsm-model", "", "unsmearing correction/unsm_corr_unc_model", 0., coFull);
	*/
	
	// TODO
	//AddMode("norm", "s", "norm/<TDIST>/g_eff", 1., coFull);

	// ---------- process modes ----------
	for (unsigned int mi = 0; mi < modes.size(); mi++)
	{
		Mode &m = modes[mi];

		string objPath = m.objName;
		size_t pos = objPath.find("<TDIST>");
		if (pos != string::npos)
			objPath = objPath.replace(pos, 7, t_dist_type);

		// load input
		m.g_input_45b = (TGraph *) f_ni_45b->Get(objPath.c_str());
		m.g_input_45t = (TGraph *) f_ni_45t->Get(objPath.c_str());

		// TODO
		/*
		if (m.fileName.compare("s") == 0)
		{
			// ROOT-bug workaround
			pos = objPath.find("/");
			string topDir = objPath.substr(0, pos);
			string rest = objPath.substr(pos+1);
			m.g_input_45b = (TGraph *) ((TDirectory *) f_ni_45b->Get(topDir.c_str()))->Get(rest.c_str());
			m.g_input_45t = (TGraph *) ((TDirectory *) f_ni_45t->Get(topDir.c_str()))->Get(rest.c_str());
		}
		*/

		if (!m.g_input_45b || !m.g_input_45t)
		{
			printf("ERROR: can't load input for mode `%s'.\n", m.tag.c_str());
			return 2;
		}

		// make combination
		m.g_eff_45b = new TGraph(); m.g_eff_45b->SetName("g_eff_45b");
		m.g_eff_45t = new TGraph(); m.g_eff_45t->SetName("g_eff_45t");
		m.g_eff_comb1 = new TGraph(); m.g_eff_comb1->SetName("g_eff_comb1");
		if (m.corr == coNo)
			{ m.g_eff_comb2 = new TGraph(); m.g_eff_comb2->SetName("g_eff_comb2"); }

		double t_min = min(t_min_45b, t_min_45t);

		for (int iidx = 0; iidx < m.g_input_45b->GetN(); iidx++)
		{
			double t, dummy;
			m.g_input_45b->GetPoint(iidx, t, dummy);

			if (t < t_min)
				continue;

			// TODO
			/*
			// full (or corrected) cross-section - gives fully reconstructed bin content
			double cs_45b = g_dsdt_45b->Eval(t);
			double cs_45t = g_dsdt_45t->Eval(t);

			// observed cross-section (with acceptance cuts) - gives number of counts per bin
			double cs_obs_45b = g_acc_dsdt_45b->Eval(t);
			double cs_obs_45t = g_acc_dsdt_45t->Eval(t);

			// acceptance factor per bin
			double A_45b = (cs_45b > 0.) ? cs_obs_45b / cs_45b : 0.;
			double A_45t = (cs_45t > 0.) ? cs_obs_45t / cs_45t : 0.;

			// weights for combination
			// w = 1 / si_stat^2, si_stat = 1/A * sqrt(cs_obs)
			// thus: w = A^2 / cs_obs;
			double w_45b = (cs_obs_45b > 0.) ? A_45b * A_45b / cs_obs_45b : 0.;
			double w_45t = (cs_obs_45t > 0.) ? A_45t * A_45t / cs_obs_45t : 0.;
			*/
			double w_45b = 1.;
			double w_45t = 1.;

			// skip points outside final acceptance region
			if (t < t_min_45b)
				w_45b = 0.;
			if (t < t_min_45t)
				w_45t = 0.;
			
			// interpolate diagonal modes, unify definition: no effect --> 0
			double m_45b = m.g_input_45b->Eval(t) - m.ref;
			double m_45t = m.g_input_45t->Eval(t) - m.ref;

			int idx = m.g_eff_45b->GetN();
			m.g_eff_45b->SetPoint(idx, t, (w_45b > 0.) ? m_45b : 0.);
			m.g_eff_45t->SetPoint(idx, t, (w_45t > 0.) ? m_45t : 0.);	

			// make combination
			// TODO
			/*
			//double S_w = w_45b + w_45t;
			double S_w_cs = w_45b*cs_45b + w_45t*cs_45t;
			//double cs_comb = (S_w > 0.) ? S_w_cs / S_w : 0.;	// combined cross-section

			double w_cs_m_45b = w_45b * cs_45b * m_45b;
			double w_cs_m_45t = w_45t * cs_45t * m_45t;
			*/

			if (m.corr == coNo)
			{
				// TODO
				//double m_comb1 = (S_w_cs > 0.) ? w_cs_m_45b / S_w_cs : 0.;
				//double m_comb2 = (S_w_cs > 0.) ? w_cs_m_45t / S_w_cs : 0.;
				m.g_eff_comb1->SetPoint(idx, t, m_45b / 2.);
				m.g_eff_comb2->SetPoint(idx, t, m_45t / 2.);
			}

			if (m.corr == coFull)
			{
				// TODO
				//double m_comb = (S_w_cs > 0.) ? (w_cs_m_45b + w_cs_m_45t) / S_w_cs : 0.;
				m.g_eff_comb1->SetPoint(idx, t, (m_45b + m_45t) / 2.);
			}

			if (m.corr == coAntiFull)
			{
				// TODO
				//double m_comb = (S_w_cs > 0.) ? (w_cs_m_45b - w_cs_m_45t) / S_w_cs : 0.;
				double m_comb = (m_45b - m_45t) / 2.;
				m.g_eff_comb1->SetPoint(idx, t, m_comb);
			}
		}

	}

	// ---------- save processed modes ----------

	TDirectory *contDir = f_out->mkdir("contributions");
	
	for (unsigned int mi = 0; mi < modes.size(); mi++)
	{
		Mode &m = modes[mi];
		gDirectory = contDir->mkdir(m.tag.c_str());
	
		m.g_input_45b->Write("g_input_45b");
		m.g_input_45t->Write("g_input_45t");

		m.g_eff_45b->SetLineColor(2); m.g_eff_45b->Write();
		m.g_eff_45t->SetLineColor(4); m.g_eff_45t->Write();
		m.g_eff_comb1->SetLineColor(8); m.g_eff_comb1->Write();

		if (m.g_eff_comb2 != NULL)
		{
			m.g_eff_comb2->SetLineColor(6);
			m.g_eff_comb2->Write();
		}
	}
	
	// ---------- build linear covariance matrices ----------

	TDirectory *matricesDir = f_out->mkdir("matrices");

	vector<string> contributions;

	// ----------

	/*

	contributions.clear();
	contributions.push_back("alig-sh-thx");
	contributions.push_back("alig-sh-thy:D+0,R+1");
	contributions.push_back("alig-sh-thy:D+1,R+0");
	MakeMatrix(contributions, matricesDir, "alig");
	
	// ----------

	contributions.clear();
	contributions.push_back("alig-sh-thy:D+0,R+1");
	contributions.push_back("alig-sh-thy:D+1,R+0");
	MakeMatrix(contributions, matricesDir, "alig-y");

	// ----------

	contributions.clear();
	contributions.push_back("thx-thy-tilt");
	MakeMatrix(contributions, matricesDir, "tilt");

	// ----------

	contributions.clear();
	contributions.push_back("opt-scale-m1");
	contributions.push_back("opt-scale-m2");
	MakeMatrix(contributions, matricesDir, "opt");

	// ----------

	contributions.clear();
	contributions.push_back("acc-corr-sigma-unc");
	contributions.push_back("acc-corr-sigma-asym");
	contributions.push_back("acc-corr-non-gauss");
	MakeMatrix(contributions, matricesDir, "acc-corr");

	// ----------

	contributions.clear();
	contributions.push_back("eff-slp");
	MakeMatrix(contributions, matricesDir, "eff-slp");

	// ----------

	contributions.clear();
	contributions.push_back("beam-mom");
	MakeMatrix(contributions, matricesDir, "beam-mom");

	// ----------

	contributions.clear();
	contributions.push_back("unsm-sigma-x");
	contributions.push_back("unsm-sigma-y");
	contributions.push_back("unsm-model");
	MakeMatrix(contributions, matricesDir, "unsm-corr");

	// ----------
	*/

	contributions.clear();
	contributions.push_back("alig-sh-thx");
	contributions.push_back("alig-sh-thy");
	contributions.push_back("thx-thy-tilt");
	contributions.push_back("opt-m1");
	contributions.push_back("opt-m2");
	contributions.push_back("acc-corr-sigma-unc-thx");
	contributions.push_back("acc-corr-sigma-unc-thy");
	//contributions.push_back("acc-corr-sigma-asym");
	//contributions.push_back("acc-corr-non-gauss");
	contributions.push_back("eff-slp");
	contributions.push_back("beam-mom");
	//contributions.push_back("unsm-sigma-x");
	//contributions.push_back("unsm-sigma-y");
	//contributions.push_back("unsm-model");
	//contributions.push_back("norm");

	MakeMatrix(contributions, matricesDir, "all-but-norm");

	delete f_out;
	return 0;
}	
