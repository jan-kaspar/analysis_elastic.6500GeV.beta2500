#include "TFile.h"
#include "TGraph.h"

#include <string>
#include <vector>
#include <map>

using namespace std;

int main()
{
	// select input
	vector<string> datasets = {
		"../DS-fill5313",
		"../DS-fill5314",
		"../DS-fill5317",
		"../DS-fill5321",
	};

	// select units
	vector<string> units = {
		"L_2_F",
		"L_1_F",
		"R_1_F",
		"R_2_F"
	};

	struct QuantityDescription
	{
		string tag;
		string label;
		double scale;
		string format;
	};

	vector<QuantityDescription> quantities = {
		{ "a", "tilt", 1., "%.1f" },
		{ "b", "shift x", 1., "%.0f" },
		{ "c", "shift y", 1., "%.0f" },
	};

	// load input
	vector<TFile*> fs_in;
	for (const auto &ds : datasets)
		fs_in.push_back(TFile::Open((ds + "/alignment_fit.root").c_str()));

	// print results - TeX formatting
	for (const auto &q : quantities)
	{
		printf("%s & ", q.label.c_str());

		for (const auto &unit : units)
		{
			bool first = true;
			for (const auto &f_in : fs_in)
			{
				TGraph *g1 = (TGraph *) f_in->Get((unit + "/" + q.tag + "_fit").c_str());
				double t, v;
				g1->GetPoint(0, t, v);

				if (!first)
					printf(", ");
			
				printf(q.format.c_str(), v * q.scale);

				first = false;
			}

			printf(" & ");
		}

		printf("\n");
	}

	printf("\n");

	// print results - parameters.h formatting
	for (unsigned int dsi = 0; dsi < datasets.size(); ++dsi)
	{
		printf("\n");
		printf("// dataset %s\n", datasets[dsi].c_str());

		TFile *f_in = fs_in[dsi];

		for (const auto &unit : units)
		{
			for (const auto &q : quantities)
			{
				TGraph *g1 = (TGraph *) f_in->Get((unit + "/" + q.tag + "_fit").c_str());
				double t, v;
				g1->GetPoint(0, t, v);

				printf("alSrc.cnst.%s_%s = %.1fE-3; ", q.tag.c_str(), unit.c_str(), v);
			}
			printf("\n");
		}
	}
}
