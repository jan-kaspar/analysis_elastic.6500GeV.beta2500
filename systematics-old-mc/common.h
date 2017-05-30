#include "TGraph.h"
#include "TSpline.h"

#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

enum DSType { dsUnknown, ds5313, ds5314, ds5317, ds5321 }; 

DSType GetDataSetType(const string &name)
{
	DSType type = dsUnknown;

	if (name.compare("DS-fill5313") == 0) type = ds5313;
	if (name.compare("DS-fill5314") == 0) type = ds5314;
	if (name.compare("DS-fill5317") == 0) type = ds5317;
	if (name.compare("DS-fill5321") == 0) type = ds5321;

	return type;
}

//----------------------------------------------------------------------------------------------------

enum DGNType { dgnUnknown, dgn_45b, dgn_45t };

DGNType GetDiagonalType(const string &name)
{
	DGNType type = dgnUnknown;

	if (name.compare("45b_56t") == 0) type = dgn_45b;
	if (name.compare("45t_56b") == 0) type = dgn_45t;

	return type;
}

//----------------------------------------------------------------------------------------------------

TGraph* CropTDistribution(TGraph *input, double cut_off = 0.3)
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
		"first fit",
		(TGraph *) GetObject("/afs/cern.ch/work/j/jkaspar/analyses/elastic/6500GeV/combined/first_fits/2/do_fit.root", "g_dsdt_CH"),
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
