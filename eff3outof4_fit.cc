#include "common_definitions.h"
#include "common_algorithms.h"
#include "parameters.h"
#include "common.h"

#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"

#include <vector>
#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if (argc != 2)
		return 1;

	// init diagonal
	Init(argv[1]);
	if (diagonal != dCombined)
		return rcIncompatibleDiagonal;
	
	// prepare output
	TFile *f_out = new TFile("eff3outof4_fit.root", "recreate");

	// combinations
	vector<string> diagonals;
	diagonals.push_back("45b_56t");
	diagonals.push_back("45t_56b");

	vector<string> units;
	units.push_back("L_2_F");
	units.push_back("L_1_F");
	units.push_back("R_1_F");
	units.push_back("R_2_F");

	// fit function
	TF1 *pol0 = new TF1("pol0", "[0]", -120E-6, +120E-6);	pol0->SetLineColor(2);
	TF1 *pol1 = new TF1("pol1", "[0] + [1]*x", -120E-6, +120E-6);	pol1->SetLineColor(8);

	for (unsigned int dgni = 0; dgni < diagonals.size(); dgni++)
	{
		printf("\n\n------------------------------ %s ------------------------------\n", diagonals[dgni].c_str());
	
		// diagonal settings
		Init(diagonals[dgni]);
		
		double th_y_min = anal.fc_G_l.th_y_0 + 5E-6;
		double th_y_max = anal.fc_G_h.th_y_0 - 3E-6;
		
		printf("th_y_min = %E\n", th_y_min);
		printf("th_y_max = %E\n", th_y_max);

		printf("\n\n");

		// get input
		string fn_in = "eff3outof4_" + diagonals[dgni] + ".root";
		TFile *f_in = new TFile(fn_in.c_str());
		if (f_in->IsZombie())
		{
			printf("ERROR: can't load file '%s'\n", fn_in.c_str());
			continue;
		}

		TDirectory *dgnDir = f_out->mkdir(diagonals[dgni].c_str());

		for (unsigned int ui = 0; ui < units.size(); ui++)
		{
			printf("\n\n>> %s\n", units[ui].c_str());

			TDirectory *unitDir = dgnDir->mkdir(units[ui].c_str());
			gDirectory = unitDir;

			TH1D *h_eff = (TH1D *) f_in->Get(("excluded RPs "+units[ui]+"/n_si 3.0/th_y dependence with th_x cut/h_refined_ratio.th_y").c_str());
			if (h_eff == NULL)
			{
				printf("ERROR: can't load h_eff for unit '%s'.\n", units[ui].c_str());
				continue;
			}
			
			h_eff->Write();

			h_eff->Fit(pol0, "", "", th_y_min, th_y_max);
			pol0->SetRange(th_y_min, th_y_max);
			pol0->Write();

			h_eff->Fit(pol1, "", "", th_y_min, th_y_max);
			pol1->SetRange(th_y_min, th_y_max);
			pol1->Write();
		}
	}

	delete f_out;
	return 0;
}
