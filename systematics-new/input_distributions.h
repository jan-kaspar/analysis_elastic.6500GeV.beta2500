#include "TSpline.h"

//----------------------------------------------------------------------------------------------------

TGraph* CropTDistribution(TGraph *input, double cut_off = 1.1)
{
	TGraph *g = new TGraph();
	for (int i = 0; i < input->GetN(); i++)
	{
		double t, v;
		input->GetPoint(i, t, v);

		if (t > cut_off)
			break;

		int idx = g->GetN();
		g->SetPoint(idx, t, v);
	}

	return g;
}

//----------------------------------------------------------------------------------------------------

TSpline* BuildSpline(TGraph *g)
{
	TSpline3 *s = new TSpline3("", g->GetX(), g->GetY(), g->GetN());
	s->SetName(g->GetName());
	return s;
}

//----------------------------------------------------------------------------------------------------

TObject* GetObject(const string &file, const string &path)
{
	TFile *f = TFile::Open(file.c_str());
	if (!f)
	{
		printf("ERROR: can't load file `%s'.\n", file.c_str());
		return NULL;
	}

	TObject *o = f->Get(path.c_str());
	if (!o)
	{
		printf("ERROR: can't load object `%s' from file `%s'.\n", path.c_str(), file.c_str());
		return NULL;
	}

	return o;
}

//----------------------------------------------------------------------------------------------------

TGraph *input_dist_t_model_bsw = NULL;
TSpline *spline_dist_t_model_bsw = NULL;

struct InputTDistribution
{
	string label;
	TGraph *g_dsdt;
	TSpline *s_dsdt;
};

vector<InputTDistribution> inputTDistributions;

//----------------------------------------------------------------------------------------------------

int LoadTDistributions()
{
	// define input t-distributions
	inputTDistributions.push_back({
		"fit2-2",
		(TGraph *) GetObject("/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/second_fit/do_fit.root", "variant 2/g_dsdt_CH"),
		NULL
	});

	inputTDistributions.push_back({
		"fit2-4",
		(TGraph *) GetObject("/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/second_fit/do_fit.root", "variant 4/g_dsdt_CH"),
		NULL
	});

	// preprocess t-distributions
	for (auto &itd : inputTDistributions)
	{
		itd.g_dsdt = CropTDistribution(itd.g_dsdt);
		itd.s_dsdt = BuildSpline(itd.g_dsdt);
	}

	return 0;
}
