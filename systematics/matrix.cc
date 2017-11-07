#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TMatrixDSym.h"
#include "TSpline.h"
#include "TF1.h"

#include "command_line_tools.h"

#include <string>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

double t_min, t_max;

//----------------------------------------------------------------------------------------------------

struct Mode
{
	string tag;
	enum SourceType { sMC, sNI, sExt } source;
	enum CorrelationType { coNo, coFull, coAntiFull } correlationType;

	// pre-processed input
	//   first index: diagonal
	//   second index: binning
	vector<vector<TH1D*>> vh_input;

	// output
	//  first index: binning
	//  second index: combination index
	vector<vector<TH1D*>> vh_combined;

	Mode(const string &_tag, SourceType _s, CorrelationType _c) : tag(_tag), source(_s), correlationType(_c)
	{
	}
};

//----------------------------------------------------------------------------------------------------

TH1D* BuildHistogramWithoutOffset(const TH1D *h_in)
{
	TH1D *h_out = new TH1D(* h_in);
	for (int bi = 1; bi <= h_out->GetNbinsX(); bi++)
	{
		const double l = h_out->GetBinLowEdge(bi);
		const double r = l + h_out->GetBinWidth(bi);

		double v = 0.;
		if (r > t_min && l < t_max)
			v = h_out->GetBinContent(bi) - 1.;

		h_out->SetBinContent(bi, v);
		h_out->SetBinError(bi, 0.);
	}

	return h_out;
}

//----------------------------------------------------------------------------------------------------

void RegularizeFirstBin(TH1D *h)
{
	int b0 = h->FindBin(0.00081);

	double diff = h->GetBinContent(b0) - h->GetBinContent(b0 + 1);

	if (fabs(diff) > 0.05)
	{
		// direct histogram fit doesn't work, a workaround here

		TGraph *g = new TGraph();
		int bm = h->FindBin(0.002);
		for (int b = b0 + 1; b < bm; ++b)
			g->SetPoint(g->GetN(), h->GetBinCenter(b), h->GetBinContent(b));

		TF1 *ff = new TF1("rf", "1 ++ x ++ x*x ++ x*x*x");
		g->Fit(ff, "Q", "");

		h->SetBinContent(b0, ff->Eval(h->GetBinCenter(b0)));

		printf("        bin %u regularised to %.4f\n", b0, ff->Eval(h->GetBinCenter(b0)));

		delete ff;
		delete g;
	}
}

//----------------------------------------------------------------------------------------------------

TSpline* BuildSpline(const TGraph *g)
{
	TSpline3 *s = new TSpline3("", g->GetX(), g->GetY(), g->GetN());
	s->SetName(g->GetName());
	return s;
}

//----------------------------------------------------------------------------------------------------

TH1D* BuildHistogramFromGraph(const TGraph *g_in, const TH1D *h_binning)
{
	// prepare output histogram
	TH1D *h_out = new TH1D(*h_binning);

	// prepare spline
	TSpline *s = BuildSpline(g_in);

	// process
	for (int bi = 1; bi <= h_out->GetNbinsX(); bi++)
	{
		const double l = h_out->GetBinLowEdge(bi);
		const double r = l + h_out->GetBinWidth(bi);

		double v = 0.;
		if (r > t_min && l < t_max)
		{
			// this algorithm ignores the variation of cross-section over the bin width
			// but it is not expected to make a big effect

			const unsigned int n_div = 100;
			const double w = (r - l) / n_div;
			double S = 0.;
			for (unsigned int i = 0; i < n_div; i++)
			{
				const double x = l + (0.5 + i) * w;
				S += s->Eval(x);
			}

			v = S / n_div - 1.;
		}

		h_out->SetBinContent(bi, v);
		h_out->SetBinError(bi, 0.);
	}

	// clean up
	delete s;

	return h_out;
}

//----------------------------------------------------------------------------------------------------

void BuildMatrix(const string &label, const vector<string> &contributions, const vector<Mode> modes, const vector<string> binnings)
{
	printf(">> BuildMatrix: %s\n", label.c_str());

	// make subdirectory
	TDirectory *d_top = gDirectory;
	TDirectory *d_matrix = d_top->mkdir(label.c_str());

	// get selected modes
	vector<const Mode *> sel_modes;
	for (unsigned int ci = 0; ci < contributions.size(); ci++)
	{
		const Mode *m = NULL;
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
	printf("    selected modes: %lu\n", sel_modes.size());

	// loop over binnings
	for (unsigned int bidx = 0; bidx < binnings.size(); ++bidx)
	{
		gDirectory = d_matrix->mkdir(binnings[bidx].c_str());

		// collect contributing histograms
		vector<TH1D *> hists;
		for (const auto sm : sel_modes)
		{
			for (const auto &h : sm->vh_combined[bidx])
				hists.push_back(h);
		}

		// extract binning information
		unsigned int bins = hists[0]->GetNbinsX();
		double *bin_edges = new double[bins + 1];
		hists[0]->GetLowEdge(bin_edges);
		bin_edges[bins] = hists[0]->GetXaxis()->GetBinUpEdge(bins);

		// build matrix
		TMatrixDSym cov_mat(bins);
		for (unsigned int i = 0; i < bins; i++)
		{
			for (unsigned int j = 0; j < bins; j++)
			{
				int bi = i + 1;
				int bj = j + 1;

				double S = 0.;
				for (const auto hist : hists)
					S += hist->GetBinContent(bi) * hist->GetBinContent(bj);

				cov_mat(i, j) = S;
			}
		}

		cov_mat.Write("cov_mat");

		// make plots from matrix
		TH1D *h_stddev = new TH1D("h_stddev", ";|t|", bins, bin_edges);
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
					double stddev = sqrt(cov_mat(i, i));
					h_stddev->SetBinContent(bi, stddev);
				}
			}
		}

		h_stddev->Write();
		h_corr_mat->Write();

		// clean up
		delete [] bin_edges;
	}

	// return to top directory
	gDirectory = d_top;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: matrix [option] [option] ...\n");
	printf("OPTIONS:\n");
	printf("    -dir-mc <directory>\n");
	printf("    -dir-ni <directory>\n");
	printf("    -output <file>\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// defaults
	string dir_mc = "data-mc/1E9";
	string dir_ni = "data-ni";

	t_min = 8E-4;
	t_max = 1.;

	string outputFile = "matrix.root";

	// parse command line
	for (int argi = 1; (argi < argc) && (cl_error == 0); ++argi)
	{
		if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0)
		{
			cl_error = 1;
			continue;
		}
		
		if (TestStringParameter(argc, argv, argi, "-dir-mc", dir_mc)) continue;
		if (TestStringParameter(argc, argv, argi, "-dir-ni", dir_ni)) continue;

		if (TestStringParameter(argc, argv, argi, "-output", outputFile)) continue;

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
	printf("    dir_mc = %s\n", dir_mc.c_str());
	printf("    dir_ni = %s\n", dir_ni.c_str());
	printf("    outputFile = %s\n", outputFile.c_str());

	// list of diagonals
	vector<string> diagonals = {
		"45b_56t",
		"45t_56b",
	};

	// list of binnings
	vector<string> binnings;
	binnings.push_back("ob-1-20-0.05");
	binnings.push_back("ob-2-10-0.05");
	binnings.push_back("ob-3-5-0.05");

	// list of modes
	vector<Mode> modes = {
		Mode("sh-thx", Mode::sNI, Mode::coFull),
		Mode("sh-thx-LRasym", Mode::sNI, Mode::coFull),

		Mode("sh-thy", Mode::sNI, Mode::coFull),
		Mode("sh-thy-LRasym", Mode::sNI, Mode::coFull),
		Mode("sh-thy-TBuncor", Mode::sNI, Mode::coNo),
		Mode("sh-thy-TBuncor-LRasym", Mode::sNI, Mode::coNo),

		Mode("tilt-thx-thy", Mode::sNI, Mode::coFull),
		Mode("tilt-thx-thy-LRasym", Mode::sMC, Mode::coFull),

		Mode("sc-thxy-mode1", Mode::sNI, Mode::coFull),
		Mode("sc-thxy-mode2", Mode::sNI, Mode::coFull),
		Mode("sc-thxy-mode3", Mode::sNI, Mode::coFull),

		Mode("dx-sigma", Mode::sNI, Mode::coFull),
		Mode("dy-sigma", Mode::sNI, Mode::coFull),
		Mode("dx-non-gauss", Mode::sNI, Mode::coFull),
		Mode("dx-mx-corr", Mode::sMC, Mode::coFull),
		Mode("dy-my-corr", Mode::sMC, Mode::coFull),

		Mode("eff-intercept", Mode::sNI, Mode::coFull),
		Mode("eff-slope", Mode::sNI, Mode::coFull),

		Mode("beam-mom", Mode::sNI, Mode::coFull),

		Mode("mx-sigma", Mode::sNI, Mode::coFull),
		Mode("my-sigma", Mode::sNI, Mode::coFull),
		Mode("unsmearing-model", Mode::sNI, Mode::coFull),

		Mode("norm", Mode::sExt, Mode::coFull),
	};

	// normalisation uncertainty
	const double norm_unc = 0.05;

	// load binning-reference histograms
	vector<TH1D *> v_binning_h;
	for (const auto &binning : binnings)
	{
		const string &diagonal = diagonals[0];
		TFile *f_in = TFile::Open((dir_mc+"/"+diagonal+"/mc_process.root").c_str());
		TH1D *h_in = (TH1D *) f_in->Get(("none/" + binning + "/h_eff").c_str());

		gDirectory = NULL;
		v_binning_h.push_back(new TH1D(*h_in));

		delete f_in;
	}

	// load input
	printf(">> loading input\n");
	for (auto &mode : modes)
	{
		printf("    %s\n", mode.tag.c_str());

		if (mode.source == Mode::sMC)
		{
			for (const auto &diagonal : diagonals)
			{
				TFile *f_in = TFile::Open((dir_mc+"/"+diagonal+"/mc_process.root").c_str());

				vector<TH1D *> v;
				for (const auto &binning : binnings)
				{
					TH1D *h_in = (TH1D *) f_in->Get((mode.tag + "/" + binning + "/h_eff").c_str());
					gDirectory = NULL;
					v.push_back(BuildHistogramWithoutOffset(h_in));
				}

				delete f_in;
			
				mode.vh_input.push_back(v);
			}
		}

		if (mode.source == Mode::sNI)
		{
			for (const auto &diagonal : diagonals)
			{
				TFile *f_in = TFile::Open((dir_ni+"/"+diagonal+"/ni_process.root").c_str());
				
				TGraph *g_in = (TGraph *) f_in->Get((mode.tag + "/g_eff").c_str());

				vector<TH1D *> v;
				for (unsigned int bi = 0; bi < binnings.size(); bi++)
				{
					gDirectory = NULL;

					TH1D *h = BuildHistogramFromGraph(g_in, v_binning_h[bi]);

					RegularizeFirstBin(h);

					v.push_back(h);
				}

				delete f_in;
			
				mode.vh_input.push_back(v);
			}
		}

		if (mode.source == Mode::sExt && mode.tag == "norm")
		{
			for (unsigned int dgni = 0; dgni < diagonals.size(); ++dgni)
			{
				vector<TH1D *> v;
				for (unsigned int bi = 0; bi < binnings.size(); bi++)
				{
					TGraph *g = new TGraph();
					g->SetPoint(0, 0., 1. + norm_unc);
					g->SetPoint(1, 1., 1. + norm_unc);

					v.push_back(BuildHistogramFromGraph(g, v_binning_h[bi]));

					delete g;
				}

				mode.vh_input.push_back(v);
			}
		}
	}

	// process
	printf(">> combining diagonals\n");
	for (auto &mode : modes)
	{
		for (unsigned int bidx = 0; bidx < binnings.size(); bidx++)
		{
			// prepare combined histograms
			vector<TH1D *> vh_com;
			if (mode.correlationType == Mode::coNo)
			{
				vh_com.push_back(new TH1D( *mode.vh_input[0][bidx]));
				vh_com.push_back(new TH1D( *mode.vh_input[0][bidx]));
			} else {
				vh_com.push_back(new TH1D( *mode.vh_input[0][bidx]));
			}

			mode.vh_combined.push_back(vh_com);

			// do combination for all bins
			for (int bin = 1; bin <= vh_com[0]->GetNbinsX(); ++bin)
			{
				const double l = mode.vh_input[0][bidx]->GetBinLowEdge(bin);
				const double r = l + mode.vh_input[0][bidx]->GetBinWidth(bin);

				// set bin to zero
				for (auto &h : vh_com)
				{
					h->SetBinContent(bin, 0.);
					h->SetBinError(bin, 0.);
				}

				// continue only if within region of interest
				if (r < t_min || l > t_max)
					continue;

				double eff_45b = mode.vh_input[0][bidx]->GetBinContent(bin);
				double eff_45t = mode.vh_input[1][bidx]->GetBinContent(bin);

				// For real data the diagonal combination (merging) is done with weight proportional to 1/sigma_stat^2.
				// However, in all datasets, both diagonals have very similar statistics and so the weights.
				// Consequently, one may assume that the merging is done as:
				//   merged = value_45 + value_56

				if (mode.correlationType == Mode::coNo)
				{
					const double m1 = eff_45b/2.;
					const double m2 = eff_45t/2.;

					vh_com[0]->SetBinContent(bin, m1);
					vh_com[1]->SetBinContent(bin, m2);
				}

				if (mode.correlationType == Mode::coFull)
				{
					const double m = (eff_45b + eff_45t) / 2.;
					vh_com[0]->SetBinContent(bin, m);
				}

				if (mode.correlationType == Mode::coAntiFull)
				{
					const double m = (eff_45b - eff_45t) / 2.;
					vh_com[0]->SetBinContent(bin, m);
				}
			}
		}
	}

	// open output
	TFile *f_out = TFile::Open(outputFile.c_str(), "recreate");

	// save modes
	TDirectory *d_contributions = f_out->mkdir("contributions");
	for (const auto &mode : modes)
	{
		TDirectory *d_mode = d_contributions->mkdir(mode.tag.c_str());

		TDirectory *d_input = d_mode->mkdir("input");
		for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
		{
			gDirectory = d_input->mkdir(diagonals[dgni].c_str());

			for (unsigned int bi = 0; bi < binnings.size(); bi++)
			{
				mode.vh_input[dgni][bi]->Write(binnings[bi].c_str());
			}
		}

		TDirectory *d_combined = d_mode->mkdir("combined");
		for (unsigned int bi = 0; bi < binnings.size(); bi++)
		{
			gDirectory = d_combined->mkdir(binnings[bi].c_str());

			for (unsigned int ci = 0; ci < mode.vh_combined[bi].size(); ci++)
			{
				char buf[100];
				sprintf(buf, "combination%u", ci);
				mode.vh_combined[bi][ci]->Write(buf);
			}
		}
	}

	// build and save matrices
	gDirectory = f_out->mkdir("matrices");

	vector<string> contributions;

	contributions = {
		"sh-thx",
		"sh-thx-LRasym",

		"sh-thy",
		"sh-thy-LRasym",
		"sh-thy-TBuncor",
		"sh-thy-TBuncor-LRasym",

		"tilt-thx-thy",
		"tilt-thx-thy-LRasym",

		"sc-thxy-mode1",
		"sc-thxy-mode2",
		"sc-thxy-mode3",

		"dx-sigma",
		"dy-sigma",
		"dx-non-gauss",
		"dx-mx-corr",
		"dy-my-corr",

		"eff-intercept",
		"eff-slope",

		"beam-mom",

		"mx-sigma",
		"my-sigma",
		"unsmearing-model",
	};

	BuildMatrix("all-but-norm", contributions, modes, binnings);

	contributions.push_back("norm");

	BuildMatrix("all", contributions, modes, binnings);

	// clean up
	delete f_out;

	return 0;
}
