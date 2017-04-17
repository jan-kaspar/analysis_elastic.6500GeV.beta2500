#include "TFile.h"
#include "TF1.h"
#include "TMath.h"

#include <vector>
#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

int main()
{
	// define input
	vector<string> datasets = {
		"DS-fill5313",
        "DS-fill5314",
        "DS-fill5317",
        "DS-fill5321"
	};

	vector<string> rps = {
		"L_2_F",
		"L_1_F",
        "R_1_F",
        "R_2_F"
	};

	vector<string> diagonals = {
		"45b_56t",
		"45t_56b"
	};

	// prepare output
	TFile *f_out = TFile::Open("global_fit.root", "recreate");

	// loop over input
	for (const auto &diagonal : diagonals)
	{
		TDirectory *d_diagonal = f_out->mkdir(diagonal.c_str());

		for (const auto &rp : rps)
		{
			TDirectory *d_rp = d_diagonal->mkdir(rp.c_str());

			double s_a=0., sw_a=0.;
			double s_b=0., sw_b=0.;
	
			for (const auto &dataset : datasets)
			{
				// skip troublesome input
				if (rp == "L_2_F" && diagonal == "45t_56b" && dataset == "DS-fill5317")
					continue;

				// get fit parameters
				TFile *f_in = TFile::Open(("../"+dataset+"/eff3outof4_fit.root").c_str());
				TF1 *ff = (TF1 *) f_in->Get((diagonal+"/"+rp+"/pol1").c_str());

				double a = ff->GetParameter(1), a_unc = ff->GetParError(1);
				double b = ff->GetParameter(0), b_unc = ff->GetParError(0);

				// set reasonable uncertainties
				b_unc = 0.005;

				// update sums
				s_a += a / a_unc / a_unc;
				sw_a += 1. / a_unc / a_unc;

				s_b += b / b_unc / b_unc;
				sw_b += 1. / b_unc / b_unc;

				// close input file
				delete f_in;
			}

			// evaluate means
			double m_a = s_a / sw_a;
			double m_a_unc = 1. / sqrt(sw_a);

			double m_b = s_b / sw_b;
			double m_b_unc = 1. / sqrt(sw_b);

			// save mean = global fit
			TF1 *ff = new TF1("ff", "[0] + [1]*x");
			ff->SetRange(4E-6, 100E-6);
			ff->SetParameter(0, m_b); ff->SetParError(0, m_b_unc);
			ff->SetParameter(1, m_a); ff->SetParError(1, m_a_unc);

			gDirectory = d_rp;
			ff->Write("pol1");
		}
	}

	// clean up
	delete f_out;

	return 0;
}
