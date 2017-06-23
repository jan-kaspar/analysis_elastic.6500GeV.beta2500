#include "TFile.h"
#include "TGraph.h"

#include <string>
#include <vector>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------------------------------

int main()
{
	// input configuration
	vector<string> datasets;
	datasets.push_back("DS-fill5313");
	datasets.push_back("DS-fill5314");
	datasets.push_back("DS-fill5317");
	datasets.push_back("DS-fill5321");

	vector<string> diagonals;
	diagonals.push_back("45b_56t");
	diagonals.push_back("45t_56b");

	// constraint configuration
	struct ConstraintInfo
	{
		string tag;
		vector<string> rot_tags;
	};

	vector<ConstraintInfo> constraintInfo = {
		{ "R1", {"-0.78540", "0.00000", "1.57080"} },
		{ "R2", {"-0.78540", "0.00000", "1.57080"} },

		{ "R3", {"1.57078", "0.00000", "1.57080"} },
		{ "R4", {"1.57078", "0.00000", "1.57080"} },

		{ "R5", {"-1.57073", "0.00000", "1.57080"} },
		{ "R6", {"-1.57073", "0.00000", "1.57080"} },

		{ "R7", {"1.57080"} },
		{ "R8", {"1.57080"} },
		{ "R9", {"1.57080"} },
		{ "R10", {"1.57080"} },
	};

	// open input file
	TFile *f_in = TFile::Open("make_fits.root");
	
	// process data
	for (const auto constraint : constraintInfo)
	{
		printf("\n\n%s\n", constraint.tag.c_str());
		
		/*
		printf("%12s", "");
		for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
			printf("%30s", diagonals[dgni].c_str());
		printf("\n");
		*/

		for (unsigned int dsi = 0; dsi < datasets.size(); dsi++)
		{
			for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
			{
				char buf[100];
				sprintf(buf, "%s, %s", datasets[dsi].c_str(), diagonals[dgni].c_str());
				printf("%20s", buf);

				// get reference R

				string path = datasets[dsi] + "/" + diagonals[dgni] + "/" + constraint.tag + "/" + constraint.rot_tags[0] + "/g_data";
				TGraph *g_data = (TGraph *) f_in->Get(path.c_str());
				double R_ref, R_ref_unc;
				g_data->GetPoint(1, R_ref, R_ref_unc);

				for (const auto &rot_tag : constraint.rot_tags)
				{
					path = datasets[dsi] + "/" + diagonals[dgni] + "/" + constraint.tag + "/" + rot_tag + "/g_data";
					g_data = (TGraph *) f_in->Get(path.c_str());
					double R, R_unc;
					g_data->GetPoint(1, R, R_unc);

					sprintf(buf, "%.4E +- %.4E", R - R_ref, R_unc);
					printf("%30s", buf);
				}
			
				printf("\n");
			}
		}
	}

	// clean up
	delete f_in;

	return 0;
}
